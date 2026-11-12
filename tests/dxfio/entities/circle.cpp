#include "entities/shapes/circle.hpp"

#include "../kind.hpp"
#include "drawn.hpp"

#include <string_view>

namespace DxfIO::Entities
{
  TEST_CASE("circle: on a layer, in a true color with its nearest index")
  {
    const Circle taken = Replayed<Circle>(Drawn("2D0"));
    CHECK(taken.Record.Layer == std::string_view{"walls"});
    CHECK(taken.Record.Color == 30);
    CHECK(taken.Record.Rgb   == 16744448);
    CHECK(taken.Radius       == 25);
  }

  TEST_CASE("circle: saved in R2000 as AutoCAD saves it, its index standing")
  {
    const Circle taken = Read<Circle>(Drawn("2D0"));
    CHECK(Written(taken, Version::R2000) == Drawn("2D0", Version::R2000));
  }

  TEST_CASE("circle: its thickness first, its extrusion after its radius")
  {
    const Circle thick = Replayed<Circle>(Drawn("2D5"));
    CHECK(thick.Thickness == 4);
    const Circle turned = Replayed<Circle>(Drawn("2D7"));
    CHECK(turned.Thickness   == 6);
    CHECK(turned.Extrusion.Y == -1);
  }

  TEST_CASE("circle: R12's, without subclasses")
  {
    const Circle taken = Replayed<Circle>(Drawn("2D5", Version::R12), Version::R12);
    CHECK(taken.Thickness == 4);
  }
} // namespace DxfIO::Entities
