#include "entities/shapes/ellipse.hpp"

#include "../kind.hpp"
#include "drawn.hpp"

namespace DxfIO::Entities
{
  TEST_CASE("ellipse: a full one, every group written, the extrusion too")
  {
    const Ellipse taken = Replayed<Ellipse>(Drawn("2F0"));
    CHECK(taken.Center    == Vector{.X = 100, .Y = 100, .Z = 0});
    CHECK(taken.Axis      == Vector{.X = 20, .Y = 0, .Z = 0});
    CHECK(taken.Extrusion == Up);
    CHECK(taken.Ratio     == 0.25);
    CHECK(taken.Start     == 0);
    CHECK(taken.End       == Turn);
  }

  TEST_CASE("ellipse: an arc of one, its parameters in radians")
  {
    const Ellipse taken = Replayed<Ellipse>(Drawn("2F1"));
    CHECK(taken.Start > 0);
    CHECK(taken.End < Turn);
  }

  TEST_CASE("ellipse: on another plane, its extrusion before its ratio")
  {
    const Ellipse taken = Replayed<Ellipse>(Drawn("2F2"));
    CHECK(taken.Extrusion.Y == -1);
    CHECK(taken.Ratio       == 0.4);
  }

  TEST_CASE("ellipse: what a record leaves out is a full circle's")
  {
    const Ellipse taken = Read<Ellipse>("  0\r\nELLIPSE\r\n100\r\nAcDbEntity\r\n  8\r\n0\r\n"
                                        "100\r\nAcDbEllipse\r\n");
    CHECK(taken.Axis      == Right);
    CHECK(taken.Ratio     == 1);
    CHECK(taken.End       == Turn);
    CHECK(taken.Extrusion == Up);
  }
} // namespace DxfIO::Entities
