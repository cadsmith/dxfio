#include "file/sections/header.hpp"

#include "tags/tags.hpp"

#include <doctest/doctest.h>

namespace DxfIO::File::Sections
{
TEST_CASE("header: a variable runs from its group 9 to the next")
{
  const auto all = Tags::All("  9\n$ACADVER\n  1\nAC1032\n"
                             "  9\n$EXTMIN\n 10\n0.0\n 20\n0.0\n 30\n0.0\n");
  REQUIRE(all);
  const auto variables = Header(*all);
  REQUIRE(variables);
  REQUIRE(variables->size() == 2);
  CHECK(variables->front().Name == "$ACADVER");
  CHECK(variables->front().Values.size() == 1);
  CHECK(variables->back().Name == "$EXTMIN");
  CHECK(variables->back().Values.size() == 3);
}

TEST_CASE("header: a body opened by another group is a failure")
{
  const auto all = Tags::All("  1\nAC1032\n");
  REQUIRE(all);
  CHECK(Header(*all) == std::unexpected{Failure{1, "header variable expected, found group 1"}});
}
} // namespace DxfIO::File::Sections
