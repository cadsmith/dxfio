#include "fields/when.hpp"

#include "fields/field.hpp"
#include "fields/numbers.hpp"
#include "fixture.hpp"
#include "tags/tags.hpp"

#include <doctest/doctest.h>

#include <tuple>

namespace DxfIO::Fields
{
namespace
{
struct Polyline
{
  int Flags{};
  double Width{};
};

auto Wide(const Polyline &one) -> bool
{
  return (one.Flags & 2) != 0;
}

const std::tuple Widths{Field{.Code = 43, .Member = &Polyline::Width}};

const std::tuple Shapes{
    Field{.Code = 70, .Member = &Polyline::Flags},
    When{.Test = Wide, .Fields = Widths},
};
} // namespace

TEST_CASE("when: fields read only when what is read so far says so")
{
  const auto narrow = Tags::All(" 70\n0\n 43\n5.0\n");
  REQUIRE(narrow);
  Walk thin{*narrow, Reading{}};
  Polyline taken{};
  Read(taken, thin, Shapes);
  CHECK(taken.Width == 0.0);
  const auto broad = Tags::All(" 70\n2\n 43\n5.0\n");
  REQUIRE(broad);
  Walk thick{*broad, Reading{}};
  Read(taken, thick, Shapes);
  CHECK(taken.Width == 5.0);
}

TEST_CASE("when: fields written only when the test holds")
{
  Sheet thin;
  REQUIRE(Write(Polyline{.Flags = 0, .Width = 5}, thin.To, Shapes));
  CHECK(thin.Out == " 70\r\n     0\r\n");
  Sheet thick;
  REQUIRE(Write(Polyline{.Flags = 2, .Width = 5}, thick.To, Shapes));
  CHECK(thick.Out == " 70\r\n     2\r\n 43\r\n5.0\r\n");
}
} // namespace DxfIO::Fields
