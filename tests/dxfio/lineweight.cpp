#include "dxfio/lineweight.hpp"

#include "dxfio/entities/polyline.hpp"
#include "dxfio/flags.hpp"
#include "dxfio/library.hpp"

#include <doctest/doctest.h>

#include <format>

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
    CHECK(Has(-1, Entities::PolylineFlags::PolyfaceMesh));
  }

  TEST_CASE("library: the version string is the three numbers, as the project declares them")
  {
    CHECK(
        LibraryVersion
        == std::format("{}.{}.{}", DXFIO_VERSION_MAJOR, DXFIO_VERSION_MINOR, DXFIO_VERSION_PATCH));
    CHECK(LibraryVersion == DXFIO_VERSION_STRING);
  }
} // namespace DxfIO
