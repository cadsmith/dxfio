#include "entities/shapes/xline.hpp"

#include "../kind.hpp"
#include "drawn.hpp"

namespace DxfIO::Entities
{
  TEST_CASE("xline: a point on it and its direction, as given")
  {
    const XLine taken = Replayed<XLine>(Drawn("2F4"));
    CHECK(taken.Start       == Vector{});
    CHECK(taken.Direction.Y == 0.4472135954999581);
  }

  TEST_CASE("xline: a record giving no direction takes the x axis")
  {
    const XLine taken = Read<XLine>("  0\r\nXLINE\r\n100\r\nAcDbEntity\r\n  8\r\n0\r\n"
                                    "100\r\nAcDbXline\r\n");
    CHECK(taken.Direction == Right);
  }
} // namespace DxfIO::Entities
