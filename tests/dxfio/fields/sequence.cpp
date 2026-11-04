#include "fields/sequence.hpp"

#include "fields/field.hpp"
#include "fields/numbers.hpp"
#include "fields/read.hpp"
#include "fixture.hpp"
#include "tags/tags.hpp"

#include <doctest/doctest.h>

#include <string_view>
#include <tuple>
#include <vector>

namespace DxfIO::Fields
{
namespace
{
struct Pair
{
  double First{};
  int Middle{};
  double Second{};
};

const std::tuple Shapes{
    Sequence{.Marker = "M"},
    Field{.Code = 40, .Member = &Pair::First},
    Field{.Code = 70, .Member = &Pair::Middle},
    Field{.Code = 40, .Member = &Pair::Second},
};
} // namespace

TEST_CASE("sequence: one code twice taken in the file's order")
{
  const auto all = Tags::All("100\nM\n 40\n1.0\n 70\n3\n 40\n2.0\n");
  REQUIRE(all);
  Walk in{*all, Reading{}};
  Pair out{};
  Read(out, in, Shapes);
  CHECK(out.First == 1);
  CHECK(out.Middle == 3);
  CHECK(out.Second == 2);
  CHECK(in.Rest().empty());
}

TEST_CASE("sequence: whole when every group is taken in order, strays aside")
{
  const auto check = [](std::string_view text) -> bool
  {
    const auto all = Tags::All(text);
    REQUIRE(all);
    Walk in{*all, Reading{}};
    Pair out{};
    Read(out, in, Shapes);
    return in.Whole();
  };
  CHECK(check("100\nM\n 40\n1.0\n 39\n1.0\n 70\n3\n 40\n2.0\n"));
  CHECK_FALSE(check("100\nM\n 70\n3\n 40\n1.0\n 40\n2.0\n"));
  CHECK_FALSE(check("100\nM\n 40\n1.0\n 70\n3\n 40\n2.0\n 90\n1\n"));
  CHECK_FALSE(check(" 40\n1.0\n 70\n3\n 40\n2.0\n"));
}

TEST_CASE("sequence: a walk never opened by one is whole")
{
  const auto all = Tags::All(" 40\n1.0\n");
  REQUIRE(all);
  const Walk in{*all, Reading{}};
  CHECK(in.Whole());
}

TEST_CASE("sequence: written with its marker")
{
  Sheet page;
  REQUIRE(Write(Pair{.First = 1, .Middle = 3, .Second = 2}, page.To, Shapes));
  CHECK(page.Out == "100\r\nM\r\n 40\r\n1.0\r\n 70\r\n     3\r\n 40\r\n2.0\r\n");
}
} // namespace DxfIO::Fields

namespace DxfIO::Fields
{
TEST_CASE("sequence: a group in its place but not fit is not taken")
{
  const auto all = Tags::All("100\nAcDbThing\n 70\n5\n");
  REQUIRE(all);
  Walk in{*all, Reading{}};
  in.Sequence("AcDbThing");
  CHECK(in.Take(70, [](const Tags::Tag &) -> bool { return false; }) == nullptr);
  CHECK_FALSE(in.Holds(70));
}
} // namespace DxfIO::Fields
