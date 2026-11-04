#include "fields/pieces.hpp"

#include "encoding/codepage.hpp"
#include "encoding/encoding.hpp"
#include "fields/read.hpp"
#include "fixture.hpp"
#include "tags/tags.hpp"

#include <doctest/doctest.h>

#include <string>
#include <tuple>

namespace DxfIO::Fields
{
namespace
{
struct MText
{
  std::string Contents;
};

const std::tuple Shapes{Pieces{.Member = &MText::Contents}};
} // namespace

TEST_CASE("pieces: joined as the file keeps them, then decoded")
{
  const auto all = Tags::All("  3\nab\\U+06\n  1\n27c\n");
  REQUIRE(all);
  Walk in{*all, Reading{}};
  MText taken{};
  Read(taken, in, Shapes);
  CHECK(taken.Contents == "ab\xd8\xa7"
                          "c");
}

TEST_CASE("pieces: 250 on group 3, the rest on group 1")
{
  Sheet page;
  REQUIRE(Write(MText{.Contents = std::string(260, 'x')}, page.To, Shapes));
  CHECK(page.Out == "  3\r\n" + std::string(250, 'x') + "\r\n  1\r\n" + std::string(10, 'x') + "\r\n");
}

TEST_CASE("pieces: every 1 and 3 joined in the order of the file")
{
  for (const std::string_view text : {"  3\na\n  1\nb\n  3\nc\n  1\nd\n", "  1\nab\n  3\ncd\n"})
  {
    const auto all = Tags::All(text);
    REQUIRE(all);
    Walk in{*all, Reading{}};
    MText taken{};
    Read(taken, in, Shapes);
    CHECK(taken.Contents == "abcd");
    CHECK(in.Rest().empty());
  }
}

TEST_CASE("pieces: the last piece may hold 250 itself")
{
  Sheet page;
  REQUIRE(Write(MText{.Contents = std::string(500, 'x')}, page.To, Shapes));
  CHECK(page.Out == "  3\r\n" + std::string(250, 'x') + "\r\n  1\r\n" + std::string(250, 'x') + "\r\n");
}

TEST_CASE("pieces: from R2007 a piece holds 250 characters, not bytes")
{
  const std::string wide = "\xf0\x9f\x98\x80";
  std::string text;
  for (int at = 0; at < 251; ++at)
  {
    text += wide;
  }
  Sheet page;
  REQUIRE(Write(MText{.Contents = text}, page.To, Shapes));
  CHECK(page.Out == "  3\r\n" + text.substr(wide.size()) + "\r\n  1\r\n" + wide + "\r\n");
  const auto all = Tags::All(page.Out);
  REQUIRE(all);
  Walk in{*all, Reading{}};
  MText taken{};
  Read(taken, in, Shapes);
  CHECK(taken.Contents == text);
}

TEST_CASE("pieces: a caret pair is one character and never cut")
{
  Sheet page;
  REQUIRE(Write(MText{.Contents = std::string(249, 'x') + "\n\ny"}, page.To, Shapes));
  CHECK(page.Out == "  3\r\n" + std::string(249, 'x') + "^J\r\n  1\r\n^Jy\r\n");
}

TEST_CASE("pieces: before R2007 by bytes, an escape cut where it falls")
{
  const Encoding::Codepage *latin = Encoding::Find("ANSI_1252");
  REQUIRE(latin != nullptr);
  Sheet page{Version::R2004, Encoding::Paged(*latin)};
  const std::string text = std::string(246, 'x') + "\xe4\xb8\xad";
  REQUIRE(Write(MText{.Contents = text}, page.To, Shapes));
  CHECK(page.Out == "  3\r\n" + std::string(246, 'x') + "\\U+4\r\n  1\r\nE2D\r\n");
  const auto all = Tags::All(page.Out);
  REQUIRE(all);
  Walk in{*all, Reading{.Release = Version::R2004, .Text = Encoding::Paged(*latin)}};
  MText taken{};
  Read(taken, in, Shapes);
  CHECK(taken.Contents == text);
}

TEST_CASE("pieces: in a page not carried a double byte that would end at byte 251 starts the next piece")
{
  Sheet page{Version::R2000, Encoding::Opaque()};
  const std::string text = std::string(249, 'x') + "\xd6\xd0y";
  REQUIRE(Write(MText{.Contents = Encoding::Decode(Encoding::Opaque(), text)}, page.To, Shapes));
  CHECK(page.Out == "  3\r\n" + std::string(249, 'x') + "\r\n  1\r\n\xd6\xd0y\r\n");
}

TEST_CASE("pieces: in a page not carried a double byte that ends at byte 250 stays in its piece")
{
  Sheet page{Version::R2000, Encoding::Opaque()};
  const std::string text = std::string(248, 'x') + "\xd6\xd0y";
  REQUIRE(Write(MText{.Contents = Encoding::Decode(Encoding::Opaque(), text)}, page.To, Shapes));
  CHECK(page.Out == "  3\r\n" + std::string(248, 'x') + "\xd6\xd0\r\n  1\r\ny\r\n");
}

TEST_CASE("pieces: a high byte last in a page not carried cuts nothing wrong")
{
  Sheet page{Version::R2000, Encoding::Opaque()};
  const std::string text = std::string(249, 'x') + "\xd6";
  REQUIRE(Write(MText{.Contents = Encoding::Decode(Encoding::Opaque(), text)}, page.To, Shapes));
  CHECK(page.Out == "  1\r\n" + text + "\r\n");
}

TEST_CASE("pieces: in a double byte page a pair that would end at byte 251 starts the next piece")
{
  const Encoding::Codepage *japanese = Encoding::Find("ANSI_932");
  REQUIRE(japanese != nullptr);
  const Encoding::Scheme with = Encoding::Paged(*japanese);
  Sheet page{Version::R2000, with};
  const std::string text = std::string(249, 'a') + "あ";
  REQUIRE(Write(MText{.Contents = text}, page.To, Shapes));
  CHECK(page.Out == "  3\r\n" + std::string(249, 'a') + "\r\n  1\r\n\x82\xa0\r\n");
  const auto all = Tags::All(page.Out);
  REQUIRE(all);
  Walk in{*all, Reading{.Release = Version::R2000, .Text = with}};
  MText taken{};
  Read(taken, in, Shapes);
  CHECK(taken.Contents == text);
}

TEST_CASE("pieces: in a double byte page a pair that ends at byte 250 stays in its piece")
{
  const Encoding::Codepage *japanese = Encoding::Find("ANSI_932");
  REQUIRE(japanese != nullptr);
  Sheet page{Version::R2000, Encoding::Paged(*japanese)};
  REQUIRE(Write(MText{.Contents = std::string(248, 'a') + "あb"}, page.To, Shapes));
  CHECK(page.Out == "  3\r\n" + std::string(248, 'a') + "\x82\xa0\r\n  1\r\nb\r\n");
}

TEST_CASE("pieces: before R2007 a caret pair counts two bytes and is never cut")
{
  Sheet page{Version::R2004, Encoding::Paged(Encoding::Standard())};
  REQUIRE(Write(MText{.Contents = std::string(249, 'x') + "\ny"}, page.To, Shapes));
  CHECK(page.Out == "  3\r\n" + std::string(249, 'x') + "\r\n  1\r\n^Jy\r\n");
}

TEST_CASE("pieces: a single high byte last in a double byte page cuts nothing wrong")
{
  const Encoding::Codepage *japanese = Encoding::Find("ANSI_932");
  REQUIRE(japanese != nullptr);
  Sheet page{Version::R2000, Encoding::Paged(*japanese)};
  REQUIRE(Write(MText{.Contents = std::string(249, 'a') + "ｱ"}, page.To, Shapes));
  CHECK(page.Out == "  1\r\n" + std::string(249, 'a') + "\xb1\r\n");
}
} // namespace DxfIO::Fields
