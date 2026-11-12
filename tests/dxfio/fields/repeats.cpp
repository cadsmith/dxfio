#include "fields/repeats.hpp"

#include <doctest/doctest.h>

namespace DxfIO::Fields
{
  TEST_CASE("repeats: a group is a repeat where a field looked for its code")
  {
    Repeats seen;
    seen.Asked(2, 6, 40);
    CHECK(seen.Repeated(3, 40));
    CHECK_FALSE(seen.Repeated(6, 40));
    CHECK_FALSE(seen.Repeated(1, 40));
    CHECK_FALSE(seen.Repeated(3, 41));
  }

  TEST_CASE("repeats: the codes the last subclass's fields asked")
  {
    Repeats seen;
    seen.Asked(0, 9, 2);
    seen.Mark();
    seen.Asked(3, 6, 40);
    CHECK(seen.Trails(40));
    CHECK_FALSE(seen.Trails(2));
  }

  TEST_CASE("repeats: a list's fences answer as one, a gap between them not")
  {
    Repeats seen;
    seen.Asked(2, 4, 331);
    seen.Asked(2, 4, 5);
    seen.Asked(4, 6, 331);
    seen.Asked(4, 6, 5);
    seen.Asked(8, 9, 331);
    CHECK(seen.Repeated(2, 331));
    CHECK(seen.Repeated(5, 331));
    CHECK(seen.Repeated(5, 5));
    CHECK_FALSE(seen.Repeated(6, 331));
    CHECK_FALSE(seen.Repeated(7, 331));
    CHECK(seen.Repeated(8, 331));
    CHECK_FALSE(seen.Repeated(8, 5));
  }

  TEST_CASE("repeats: a look is not joined across a subclass")
  {
    Repeats seen;
    seen.Asked(0, 3, 40);
    seen.Mark();
    seen.Asked(3, 6, 40);
    CHECK(seen.Trails(40));
    CHECK(seen.Repeated(1, 40));
    CHECK(seen.Repeated(4, 40));
  }
} // namespace DxfIO::Fields
