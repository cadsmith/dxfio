#include "file/sections/records.hpp"

#include "tags/tags.hpp"

#include <doctest/doctest.h>

namespace DxfIO::File::Sections
{
  TEST_CASE("records: a body is its records, each from its group 0")
  {
    const auto all = Tags::All("  0\nLINE\n  8\n0\n  0\nPOINT\n 10\n1.0\n");
    REQUIRE(all);
    const auto found = Records(*all);
    REQUIRE(found);
    REQUIRE(found->size()            == 2);
    CHECK(found->front().Tags.size() == 2);
    CHECK(found->back().Name         == "POINT");
  }

  TEST_CASE("records: a body opened by another group is a failure")
  {
    const auto all = Tags::All("  8\n0\n");
    REQUIRE(all);
    CHECK(Records(*all) == std::unexpected{Failure{1, "group 0 expected, found group 8"}});
  }
} // namespace DxfIO::File::Sections
