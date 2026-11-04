#include "fields/first.hpp"

#include "fields/field.hpp"
#include "fields/numbers.hpp"
#include "fields/subclass.hpp"
#include "fixture.hpp"
#include "tags/tags.hpp"

#include <doctest/doctest.h>

#include <tuple>

namespace DxfIO::Fields
{
namespace
{
struct Style
{
  int Fill{};
  int Color{};
};

const std::tuple Shapes{
    Subclass{.Marker = "AcDbStyle"},
    First{.Code = 62, .Fields = std::tuple{Field{.Code = 62, .Member = &Style::Fill}}},
    Field{.Code = 62, .Member = &Style::Color},
};
} // namespace

TEST_CASE("first: a code's first group to it, the rest to the fields after")
{
  const auto all = Tags::All("100\nAcDbStyle\n 62\n1\n 49\n0.5\n 62\n2\n");
  REQUIRE(all);
  Walk in{*all, Reading{}};
  Style taken{};
  Read(taken, in, Shapes);
  CHECK(taken.Fill == 1);
  CHECK(taken.Color == 2);
}

TEST_CASE("first: from the fence's start, wherever the last field stopped")
{
  const auto all = Tags::All("100\nAcDbStyle\n 62\n1\n  8\na\n 62\n2\n");
  REQUIRE(all);
  Walk in{*all, Reading{}};
  REQUIRE(in.Subclass("AcDbStyle"));
  CHECK(in.Take(8) != nullptr);
  Style taken{};
  Take(First{.Code = 62, .Fields = std::tuple{Field{.Code = 62, .Member = &Style::Fill}}}, taken, in);
  CHECK(taken.Fill == 1);
}

TEST_CASE("first: written as its fields")
{
  Sheet page;
  const Style held{.Fill = 3, .Color = 0};
  REQUIRE(Put(First{.Code = 62, .Fields = std::tuple{Field{.Code = 62, .Member = &Style::Fill}}}, held, page.To));
  CHECK(page.Out == " 62\r\n     3\r\n");
}
} // namespace DxfIO::Fields
