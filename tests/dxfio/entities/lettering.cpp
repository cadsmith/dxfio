#include "entities/shapes/lettering.hpp"

#include "../fields/fixture.hpp"
#include "fields/read.hpp"
#include "tags/tags.hpp"

#include <doctest/doctest.h>

#include <string_view>

namespace DxfIO::Entities
{
TEST_CASE("lettering: a new one is its point, height and value, nothing else")
{
  Fields::Sheet page;
  Lettering fresh{};
  fresh.Height = 1;
  fresh.Value = "x";
  REQUIRE(Fields::Write(fresh, page.To, Shapes<Lettering>));
  CHECK(page.Out == "100\r\nAcDbText\r\n 10\r\n0.0\r\n 20\r\n0.0\r\n 30\r\n0.0\r\n"
                    " 40\r\n1.0\r\n  1\r\nx\r\n");
}

TEST_CASE("lettering: every group in AutoCAD's order, as they are read")
{
  constexpr std::string_view text = "100\r\nAcDbText\r\n 39\r\n2.0\r\n 10\r\n1.0\r\n 20\r\n2.0\r\n "
                                    "30\r\n3.0\r\n"
                                    " 40\r\n0.5\r\n  1\r\nvalue\r\n 50\r\n30.0\r\n 41\r\n0.8\r\n "
                                    "51\r\n10.0\r\n"
                                    "  7\r\ntall\r\n 71\r\n     2\r\n 72\r\n     1\r\n"
                                    " 11\r\n4.0\r\n 21\r\n5.0\r\n 31\r\n6.0\r\n"
                                    "210\r\n0.0\r\n220\r\n0.0\r\n230\r\n-1.0\r\n";
  const auto all = Tags::All(text);
  REQUIRE(all);
  Fields::Walk in{*all, Fields::Reading{}};
  Lettering taken{};
  Fields::Read(taken, in, Shapes<Lettering>);
  CHECK(taken.Thickness == 2);
  CHECK(taken.Value == "value");
  CHECK(taken.Width == 0.8);
  CHECK(taken.Style == "tall");
  CHECK(taken.Generation == 2);
  CHECK(taken.Alignment == Vector{.X = 4, .Y = 5, .Z = 6});
  CHECK(taken.Extrusion.Z == -1);
  Fields::Sheet page;
  REQUIRE(Fields::Write(taken, page.To, Shapes<Lettering>));
  CHECK(page.Out == text);
}
} // namespace DxfIO::Entities
