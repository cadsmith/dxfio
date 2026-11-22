#include "fields/text.hpp"

#include "fixture.hpp"
#include "tags/tags.hpp"

#include <doctest/doctest.h>

#include <string>
#include <string_view>

namespace DxfIO::Fields
{
  TEST_CASE("text: decoded from the file's page, encoded back")
  {
    const auto all = Tags::All("  1\ncaf\xe9\n");
    REQUIRE(all);
    const Encoding::Scheme latin = Encoding::Paged(Encoding::Standard());
    Walk                   in{*all, Reading{.Release = Version::R2004, .Text = latin}};
    CHECK(Value<std::string>::Read(in, 1) == "caf\xc3\xa9");
    CHECK_FALSE(Value<std::string>::Read(in, 1));
    Sheet page{Version::R2004, latin};
    REQUIRE(Value<std::string>::Write(page.To, 1, "caf\xc3\xa9"));
    CHECK(page.Out == "  1\r\ncaf\xe9\r\n");
  }

  TEST_CASE("text: a word written as it is, escapes and all")
  {
    Sheet page;
    REQUIRE(Word(page.To, 0, "\\U+004CINE"));
    CHECK(page.Out == "  0\r\n\\U+004CINE\r\n");
  }

  TEST_CASE("text: a word's line end kept off the line")
  {
    Sheet page;
    REQUIRE(Word(page.To, 2, "A\r\nB"));
    CHECK(page.Out == "  2\r\nA^M^JB\r\n");
  }

  TEST_CASE(
      "text: a word's NUL goes out as a caret, in either form; other control bytes as they are")
  {
    using namespace std::string_view_literals;
    Sheet page;
    REQUIRE(Word(page.To, 0, "FO\0O\tX"sv));
    CHECK(page.Out == "  0\r\nFO^@O^IX\r\n");
    std::string     packed;
    Tags::Writer    writer{packed, Tags::Form::Narrow};
    Fields::Writing to{.To = &writer, .Release = Version::R12};
    REQUIRE(Word(to, 0, "FO\0O"sv));
    CHECK(packed == "\0FO^@O\0"sv);
  }

  TEST_CASE("text: a name read as a symbol and written back")
  {
    const auto all = Tags::All("  8\nwalls\n");
    REQUIRE(all);
    Walk in{*all, Reading{}};
    CHECK(Value<Symbol>::Read(in, 8) == Symbol{"walls"});
    Sheet page;
    REQUIRE(Value<Symbol>::Write(page.To, 8, Symbol{"walls"}));
    CHECK(page.Out == "  8\r\nwalls\r\n");
  }
} // namespace DxfIO::Fields
