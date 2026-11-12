#include "entities/shapes/point.hpp"

#include "../kind.hpp"
#include "drawn.hpp"

namespace DxfIO::Entities
{
  TEST_CASE("point: where it is, its thickness after")
  {
    const Point plain = Replayed<Point>(Drawn("2D3"));
    CHECK(plain.Position == Vector{.X = 10, .Y = 10, .Z = 0});
    const Point thick = Replayed<Point>(Drawn("2D4"));
    CHECK(thick.Position.Z == 3);
    CHECK(thick.Thickness  == 2);
  }

  TEST_CASE("point: its extrusion, then its x axis angle, when it has them")
  {
    const Point taken = Replayed<Point>(Drawn("2D9"));
    CHECK(taken.Extrusion == Vector{.X = 0, .Y = 1, .Z = 0});
    CHECK(taken.Angle > 0);
  }

  TEST_CASE("point: R12's, without subclasses")
  {
    const Point taken = Replayed<Point>(Drawn("2D4", Version::R12), Version::R12);
    CHECK(taken.Thickness == 2);
  }
} // namespace DxfIO::Entities
