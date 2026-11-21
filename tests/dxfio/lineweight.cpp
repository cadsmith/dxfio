#include "dxfio/lineweight.hpp"

#include "dxfio/entities/polyline.hpp"
#include "dxfio/flags.hpp"

#include <doctest/doctest.h>

namespace DxfIO
{
  TEST_CASE("lineweight: each standard width gives its millimeters, the names and others none")
  {
    CHECK(MillimetersOf(Lineweight::Mm000) == 0.0);
    CHECK(MillimetersOf(Lineweight::Mm025) == doctest::Approx(0.25));
    CHECK(MillimetersOf(Lineweight::Mm211) == doctest::Approx(2.11));
    CHECK_FALSE(MillimetersOf(Lineweight::ByLayer));
    CHECK_FALSE(MillimetersOf(Lineweight::ByBlock));
    CHECK_FALSE(MillimetersOf(Lineweight::Default));
    CHECK_FALSE(MillimetersOf(static_cast<Lineweight>(27)));
  }

  TEST_CASE("lineweight: a flag is had when every one of its bits is set")
  {
    const int flags = 1 | 8;
    CHECK(Has(flags, Entities::PolylineFlags::Closed));
    CHECK(Has(flags, Entities::PolylineFlags::Polyline3d));
    CHECK_FALSE(Has(flags, Entities::PolylineFlags::PolyfaceMesh));
  }

} // namespace DxfIO
