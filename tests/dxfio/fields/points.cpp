#include "fields/points.hpp"

#include "fixture.hpp"
#include "tags/tags.hpp"

#include <doctest/doctest.h>

namespace DxfIO::Fields
{
TEST_CASE("points: a point's three reals, each when given")
{
  const auto all = Tags::All(" 10\n1.0\n 20\n2.0\n 30\n3.0\n 31\n7.0\n");
  REQUIRE(all);
  Walk in{*all, Reading{}};
  CHECK(Value<Vector>::Read(in, 10) == Vector{.X = 1, .Y = 2, .Z = 3});
  CHECK(Value<Vector>::Read(in, 11) == Vector{.X = 0, .Y = 0, .Z = 7});
  CHECK_FALSE(Value<Vector>::Read(in, 12));
}

TEST_CASE("points: written as its three groups")
{
  Sheet page;
  REQUIRE(Value<Vector>::Write(page.To, 10, Vector{.X = 1, .Y = 2, .Z = 0}));
  CHECK(page.Out == " 10\r\n1.0\r\n 20\r\n2.0\r\n 30\r\n0.0\r\n");
}
} // namespace DxfIO::Fields

namespace DxfIO::Fields
{
TEST_CASE("points: a point given only its y")
{
  const auto all = Tags::All(" 20\n2.0\n");
  REQUIRE(all);
  Walk in{*all, Reading{}};
  CHECK(Value<Vector>::Read(in, 10) == Vector{.Y = 2});
}
} // namespace DxfIO::Fields
