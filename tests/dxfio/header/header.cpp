#include "dxfio/header/header.hpp"

#include "../fields/fixture.hpp"
#include "encoding/codepage.hpp"
#include "file/sections/header.hpp"
#include "header/io.hpp"
#include "tags/tags.hpp"

#include <doctest/doctest.h>

#include <string>
#include <string_view>
#include <vector>

namespace DxfIO::Header
{
TEST_CASE("header: the release and the page a header declares")
{
  const auto all = Tags::All("  9\n$ACADVER\n  1\nAC1015\n"
                             "  9\n$DWGCODEPAGE\n  3\nANSI_1256\n");
  REQUIRE(all);
  const auto variables = File::Sections::Header(*all);
  REQUIRE(variables);
  CHECK(Declared(*variables) == Version::R2000);
  CHECK(Named(*variables).Name == "ANSI_1256");
  CHECK(Declared(std::span<const File::Variable>{}) == Version::R12);
  CHECK(Named(std::span<const File::Variable>{}).Name == "ANSI_1252");
}

TEST_CASE("header: the release only from group 1, read or loaded alike")
{
  const auto all = Tags::All("  9\n$ACADVER\n  5\nAC1032\n");
  REQUIRE(all);
  const auto variables = File::Sections::Header(*all);
  REQUIRE(variables);
  CHECK(Declared(*variables) == Version::R12);
  CHECK(Declared(Read(*variables, Fields::Reading{})) == Version::R12);
}

TEST_CASE("header: what the release is read from kept as it is, escapes too")
{
  const auto all = Tags::All("  9\n$ACADVER\n  1\n\\U+0041C1032\n"
                             "  9\n$\\U+0041BC\n  1\n\\U+0041BC\n");
  REQUIRE(all);
  const auto variables = File::Sections::Header(*all);
  REQUIRE(variables);
  const std::vector<Variable> kept = Read(*variables, Fields::Reading{.Text = Encoding::Utf8()});
  CHECK(Declared(*variables) == Version::R12);
  CHECK(Declared(kept) == Version::R12);
  REQUIRE(kept.size() == 2);
  CHECK(kept.front().Values.front() == Group{.Code = 1, .Value = std::string{"\\U+0041C1032"}});
  CHECK(kept.back().Name == "$\\U+0041BC");
  CHECK(kept.back().Values.front() == Group{.Code = 1, .Value = std::string{"ABC"}});
}

TEST_CASE("header: a page name that is no page written as it was read")
{
  const auto all = Tags::All("  9\n$DWGCODEPAGE\n  3\n\x01\n");
  REQUIRE(all);
  const auto variables = File::Sections::Header(*all);
  REQUIRE(variables);
  const std::vector<Variable> kept = Read(*variables, Fields::Reading{.Text = Encoding::Utf8()});
  Fields::Sheet page{Version::R2018};
  REQUIRE(Write(page.To, kept));
  CHECK(page.Out.contains("  3\r\n\x01\r\n"));
}

TEST_CASE("header: the variables read and written back")
{
  const std::string_view text = "  9\r\n$ACADVER\r\n  1\r\nAC1015\r\n"
                                "  9\r\n$DWGCODEPAGE\r\n  3\r\nANSI_1256\r\n";
  const auto all = Tags::All(text);
  REQUIRE(all);
  const auto variables = File::Sections::Header(*all);
  REQUIRE(variables);
  const std::vector<Variable> taken = Read(*variables, Fields::Reading{});
  CHECK(Named(taken).Name == "ANSI_1256");
  Fields::Sheet page{Version::R2000};
  REQUIRE(Write(page.To, taken));
  CHECK(page.Out == text);
}

TEST_CASE("header: written with the release and the page written")
{
  const std::vector<Variable> all{
      Variable{.Name = "$ACADVER", .Values = {Group{.Code = 1, .Value = std::string{"AC1032"}}}},
      Variable{.Name = "$DWGCODEPAGE", .Values = {Group{.Code = 3, .Value = std::string{"ANSI_1252"}}}},
  };
  const Encoding::Codepage *cyrillic = Encoding::Find("ANSI_1251");
  REQUIRE(cyrillic != nullptr);
  Fields::Sheet old{Version::R12, Encoding::Paged(*cyrillic)};
  REQUIRE(Write(old.To, all));
  CHECK(old.Out == "  9\r\n$ACADVER\r\n  1\r\nAC1009\r\n"
                   "  9\r\n$DWGCODEPAGE\r\n  3\r\nANSI_1251\r\n");
  Fields::Sheet modern{Version::R2018};
  REQUIRE(Write(modern.To, all));
  CHECK(modern.Out == "  9\r\n$ACADVER\r\n  1\r\nAC1032\r\n"
                      "  9\r\n$DWGCODEPAGE\r\n  3\r\nANSI_1252\r\n");
}

TEST_CASE("header: saved down to R13 or R14, the page its text is in is named")
{
  const std::vector<Variable> all{
      Variable{.Name = "$ACADVER", .Values = {Group{.Code = 1, .Value = std::string{"AC1032"}}}},
  };
  const Encoding::Codepage *cyrillic = Encoding::Find("ANSI_1251");
  REQUIRE(cyrillic != nullptr);
  for (const Version release : {Version::R13, Version::R14})
  {
    CAPTURE(release);
    Fields::Sheet down{release, Encoding::Paged(*cyrillic)};
    down.To.From = Version::R2018;
    REQUIRE(Write(down.To, all));
    CHECK(down.Out.contains("$DWGCODEPAGE\r\n  3\r\nANSI_1251\r\n"));
  }
}
} // namespace DxfIO::Header

namespace DxfIO::Header
{
TEST_CASE("header: a code page held as no text written as it is held")
{
  Fields::Sheet page{Version::R2018};
  const std::vector<Variable> all{
      Variable{.Name = "$DWGCODEPAGE", .Values = {Group{.Code = 3, .Value = std::int64_t{5}}}}};
  REQUIRE(Write(page.To, all));
  const std::size_t at = page.Out.find("$DWGCODEPAGE");
  REQUIRE(at != std::string::npos);
  CHECK(page.Out.substr(at).starts_with("$DWGCODEPAGE\r\n  3\r\n     5\r\n"));
}

TEST_CASE("header: a release or a page held as no text names none")
{
  const std::vector<Variable> all{
      Variable{.Name = "$ACADVER", .Values = {Group{.Code = 1, .Value = std::int64_t{1}}}},
      Variable{.Name = "$DWGCODEPAGE", .Values = {Group{.Code = 3, .Value = std::int64_t{5}}}},
  };
  CHECK(Declared(all) == Version::R12);
  CHECK(Named(all).Name == "ANSI_1252");
  CHECK_FALSE(Opaque(all));
}
} // namespace DxfIO::Header
