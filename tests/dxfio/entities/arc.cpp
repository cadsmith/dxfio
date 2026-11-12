#include "entities/shapes/arc.hpp"

#include "../kind.hpp"
#include "drawn.hpp"

namespace DxfIO::Entities
{
  TEST_CASE("arc: a circle's groups then its angles, always, even at 0")
  {
    const Arc taken = Replayed<Arc>(Drawn("2D1"));
    CHECK(taken.Record.Transparency == 33554559);
    CHECK(taken.Center == Vector{.X = 50, .Y = 0, .Z = 0});
    CHECK(taken.Start  == 0);
    CHECK(taken.End    == 180);
  }

  TEST_CASE("arc: its thickness first, its extrusion inside AcDbCircle")
  {
    const Arc thick = Replayed<Arc>(Drawn("2D6"));
    CHECK(thick.Thickness == 3);
    const Arc turned = Replayed<Arc>(Drawn("2D8"));
    CHECK(turned.Thickness   == 2);
    CHECK(turned.Extrusion.Y == -1);
  }

  TEST_CASE("arc: R12's, without subclasses")
  {
    const Arc taken = Replayed<Arc>(Drawn("2D2", Version::R12), Version::R12);
    CHECK(taken.End == 90);
  }
} // namespace DxfIO::Entities
