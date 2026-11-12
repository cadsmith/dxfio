#include "ascii.hpp"

#include <doctest/doctest.h>

namespace DxfIO
{
  static_assert(Ascii::Upper('a') == 'A' && Ascii::Upper('z') == 'Z');
  static_assert(Ascii::Upper('A') == 'A' && Ascii::Upper('`') == '`' && Ascii::Upper('{') == '{');
  static_assert(Ascii::Lower('A') == 'a' && Ascii::Lower('Z') == 'z');
  static_assert(Ascii::Lower('a') == 'a' && Ascii::Lower('@') == '@' && Ascii::Lower('[') == '[');

  TEST_CASE("ascii: lower folds ASCII letters and nothing else")
  {
    CHECK(Ascii::Lower('A')    == 'a');
    CHECK(Ascii::Lower('z')    == 'z');
    CHECK(Ascii::Lower('1')    == '1');
    CHECK(Ascii::Lower('\xc9') == '\xc9');
  }

  TEST_CASE("ascii: only ASCII letters change case, whole texts compared")
  {
    CHECK(Ascii::Upper('\xe9') == '\xe9');
    CHECK(Ascii::Same("Line", "LINE"));
    CHECK(Ascii::Same("", ""));
    CHECK_FALSE(Ascii::Same("LIN", "LINE"));
    CHECK_FALSE(Ascii::Same("LINES", "LINE"));
    CHECK_FALSE(Ascii::Same("\xc3\xa9", "\xc3\x89"));
  }
} // namespace DxfIO

namespace DxfIO
{
  TEST_CASE("ascii: a sign past the small letters keeps its case")
  {
    CHECK(Ascii::Upper('{') == '{');
  }
} // namespace DxfIO
