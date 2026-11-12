#include "fields/budget.hpp"

#include <doctest/doctest.h>

#include <cstdint>

namespace DxfIO::Fields
{
  TEST_CASE("budget: counts claim no more than the record holds")
  {
    Budget left{5};
    CHECK(left.Claim(3)       == 3);
    CHECK(left.Claim(1000000) == 2);
    CHECK(left.Claim(1)       == 0);
  }

  TEST_CASE("budget: a count below 1 claims none")
  {
    Budget left{5};
    CHECK(left.Claim(0)  == 0);
    CHECK(left.Claim(-7) == 0);
    CHECK(left.Claim(5)  == 5);
  }

  TEST_CASE("budget: a count past 32 bits claims what is left, wherever built")
  {
    Budget left{5};
    CHECK(left.Claim(std::int64_t{1} << 32) == 5);
  }
} // namespace DxfIO::Fields
