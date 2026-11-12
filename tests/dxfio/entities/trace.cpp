#include "entities/shapes/trace.hpp"

#include "../kind.hpp"
#include "drawn.hpp"

namespace DxfIO::Entities
{
  TEST_CASE("trace: a solid's groups under its own name")
  {
    const Trace plain = Replayed<Trace>(Drawn("2F8"));
    CHECK(plain.Corners.at(1) == Vector{.X = 0, .Y = 51, .Z = 0});
    const Trace thick = Replayed<Trace>(Drawn("2F9"));
    CHECK(thick.Thickness == 1);
  }

  TEST_CASE("trace: R12's, without subclasses")
  {
    const Trace taken = Replayed<Trace>(Drawn("2F9", Version::R12), Version::R12);
    CHECK(taken.Thickness == 1);
  }
} // namespace DxfIO::Entities
