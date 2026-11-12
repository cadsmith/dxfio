#include "encoding/escapes.hpp"

#include <doctest/doctest.h>

#include <string>

namespace DxfIO::Encoding
{
  TEST_CASE("escapes: four hex digits of either case after \\U+")
  {
    const auto upper = Leading("\\U+0627z");
    REQUIRE(upper);
    CHECK(upper->Point  == U'ا');
    CHECK(upper->Length == 7);
    const auto lower = Leading("\\U+00e9");
    REQUIRE(lower);
    CHECK(lower->Point == U'é');
  }

  TEST_CASE("escapes: a surrogate pair's two escapes are one point")
  {
    const auto pair = Leading("\\U+D83D\\U+DE00");
    REQUIRE(pair);
    CHECK(pair->Point  == U'\U0001F600');
    CHECK(pair->Length == 14);
  }

  TEST_CASE("escapes: \\M+ names a page by its number, then its two bytes")
  {
    const auto japanese = Leading("\\M+182A0rest");
    REQUIRE(japanese);
    CHECK(japanese->Point             == U'\u3042');
    CHECK(japanese->Length            == 8);
    CHECK(Leading("\\M+5D6D0")->Point == U'\u4e2d');
    CHECK(Leading("\\M+2a4a4")->Point == U'\u4e2d');
    CHECK(Leading("\\M+3B0A1")->Point == U'\uac00');
    CHECK(Leading("\\M+48861")->Point == U'\uac00');
    CHECK_FALSE(Leading("\\M+082A0"));
    CHECK_FALSE(Leading("\\M+682A0"));
    CHECK_FALSE(Leading("\\M+182G0"));
    CHECK_FALSE(Leading("\\M+18100"));
    CHECK_FALSE(Leading("\\M+182A"));
  }

  TEST_CASE("escapes: what is no escape stays text")
  {
    CHECK_FALSE(Leading("\\U+12G4"));
    CHECK_FALSE(Leading("\\U+12"));
    CHECK_FALSE(Leading("\\u+0627"));
    CHECK_FALSE(Leading("a\\U+0627"));
    CHECK_FALSE(Leading("\\U+D83D"));
    CHECK_FALSE(Leading("\\U+D83Dz"));
    CHECK_FALSE(Leading("\\U+DE00"));
  }

  TEST_CASE("escapes: a point appended as an escape, a pair past U+FFFF")
  {
    std::string out;
    Escaped(out, U'ا');
    Escaped(out, U'\U0001F600');
    CHECK(out == "\\U+0627\\U+D83D\\U+DE00");
  }
} // namespace DxfIO::Encoding

namespace DxfIO::Encoding
{
  TEST_CASE("escapes: past the surrogates a point is its own, a broken pair none")
  {
    CHECK_FALSE(Leading("\\U+GGGG"));
    REQUIRE(Leading("\\U+E000"));
    CHECK(Leading("\\U+E000")->Point == U'\ue000');
    CHECK_FALSE(Leading("\\U+D83D\\U+0041"));
    CHECK_FALSE(Leading("\\U+D83D\\U+E000"));
  }
} // namespace DxfIO::Encoding
