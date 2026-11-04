#include "fields/count.hpp"

#include "fields/read.hpp"
#include "fixture.hpp"
#include "tags/tags.hpp"

#include <doctest/doctest.h>

#include <tuple>
#include <vector>

namespace DxfIO::Fields
{
namespace
{
struct Knots
{
  std::vector<double> Values;
};

const std::tuple Shapes{Count{.Code = 72, .Member = &Knots::Values}};
} // namespace

TEST_CASE("count: a list made as long as its count, out of what is held")
{
  const auto all = Tags::All(" 72\n2\n 40\n0.0\n 40\n1.0\n");
  REQUIRE(all);
  Walk in{*all, Reading{}};
  Knots taken{};
  Read(taken, in, Shapes);
  CHECK(taken.Values.size() == 2);
}

TEST_CASE("count: a count below 1 or missing makes an empty list")
{
  const auto all = Tags::All(" 72\n-3\n");
  REQUIRE(all);
  Walk in{*all, Reading{}};
  Knots taken{.Values = {1.0}};
  Read(taken, in, Shapes);
  CHECK(taken.Values.empty());
}

TEST_CASE("count: written as the list's size")
{
  Sheet page;
  REQUIRE(Write(Knots{.Values = {1.0, 2.0, 3.0}}, page.To, Shapes));
  CHECK(page.Out == " 72\r\n     3\r\n");
}
} // namespace DxfIO::Fields
