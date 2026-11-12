#include "entities/shapes/solid.hpp"

#include "../kind.hpp"
#include "drawn.hpp"

namespace DxfIO::Entities
{
  TEST_CASE("solid: four corners in the file's order")
  {
    const Solid taken = Replayed<Solid>(Drawn("2F5"));
    CHECK(taken.Corners.at(1) == Vector{.X = 310, .Y = 0, .Z = 0});
    CHECK(taken.Corners.at(3) == Vector{.X = 310, .Y = 10, .Z = 0});
    CHECK(taken.Thickness     == 0);
    CHECK(taken.Extrusion     == Up);
  }

  TEST_CASE("solid: a triangle's fourth corner its third, its thickness after")
  {
    const Solid taken = Replayed<Solid>(Drawn("2F6"));
    CHECK(taken.Corners.at(3) == taken.Corners.at(2));
    CHECK(taken.Thickness     == 3);
  }

  TEST_CASE("solid: its extrusion after its corners")
  {
    const Solid taken = Replayed<Solid>(Drawn("2F7"));
    CHECK(taken.Extrusion.Y == -1);
  }

  TEST_CASE("solid: R12's, without subclasses")
  {
    const Solid taken = Replayed<Solid>(Drawn("2F6", Version::R12), Version::R12);
    CHECK(taken.Thickness == 3);
  }
} // namespace DxfIO::Entities
