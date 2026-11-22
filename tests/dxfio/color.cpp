#include "dxfio/color.hpp"

#include "dxfio/colors.hpp"

#include <doctest/doctest.h>

namespace DxfIO
{
  TEST_CASE("color: the method from the index, unless a true color is given")
  {
    Entities::Record record{};
    CHECK(MethodOf(record) == ColorMethod::ByLayer);
    record.Color = Colors::ByBlock;
    CHECK(MethodOf(record) == ColorMethod::ByBlock);
    record.Color = 1;
    CHECK(MethodOf(record) == ColorMethod::Index);
    CHECK_FALSE(RgbOf(record));
    record.Rgb = 0x00FF8000;
    CHECK(MethodOf(record) == ColorMethod::Rgb);
    CHECK(RgbOf(record)    == 0xFF8000U);
  }

  TEST_CASE("color: a transparency's method from its top byte, its alpha from its low one")
  {
    Entities::Record record{};
    CHECK_FALSE(TransparencyMethodOf(record));
    CHECK_FALSE(AlphaOf(record));
    record.Transparency = 0x020000E5;
    CHECK(TransparencyMethodOf(record) == TransparencyMethod::Alpha);
    CHECK(AlphaOf(record) == 0xE5);
    record.Transparency = 0x01000000;
    CHECK(TransparencyMethodOf(record) == TransparencyMethod::ByBlock);
    CHECK_FALSE(AlphaOf(record));
    record.Transparency = 0;
    CHECK(TransparencyMethodOf(record) == TransparencyMethod::ByLayer);
    record.Transparency = 0x07000000;
    CHECK_FALSE(TransparencyMethodOf(record));
    CHECK_FALSE(AlphaOf(record));
  }
} // namespace DxfIO
