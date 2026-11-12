#include "tables/shapes/layer.hpp"

#include "../kind.hpp"

#include <string_view>

namespace DxfIO::Tables
{
  namespace
  {
    constexpr std::string_view Text =
        "  0\r\nLAYER\r\n  5\r\n10\r\n330\r\n2\r\n100\r\nAcDbSymbolTableRecord\r\n"
        "100\r\nAcDbLayerTableRecord\r\n  2\r\nwalls\r\n 70\r\n     4\r\n"
        " 62\r\n    -1\r\n  6\r\nDASHED\r\n290\r\n     0\r\n370\r\n    35\r\n"
        "390\r\nF\r\n347\r\n2A\r\n348\r\n0\r\n";
  } // namespace

  TEST_CASE("layer: read from AutoCAD's record and written back the same")
  {
    const Layer taken = Read<Layer>(Text);
    CHECK(taken.Record.Name  == "walls");
    CHECK(taken.Record.Flags == 4);
    CHECK(taken.Color        == -1);
    CHECK(taken.Linetype     == "DASHED");
    CHECK_FALSE(taken.Plot);
    CHECK(taken.Lineweight == 35);
    CHECK(taken.Style      == Handle{0xf});
    CHECK(taken.Material   == Handle{0x2a});
    CHECK_FALSE(taken.Rgb);
    CHECK_FALSE(taken.Spare);
    CHECK(taken.Record.Base.Extra.empty());
    CHECK(Written(taken) == Text);
  }

  TEST_CASE("layer: a layer that plots leaves out its plot flag")
  {
    const Layer plotted =
        Read<Layer>("  0\nLAYER\n100\nAcDbSymbolTableRecord\n100\nAcDbLayerTableRecord\n"
                    "  2\n0\n 70\n0\n 62\n7\n420\n16711680\n  6\nContinuous\n370\n-3\n");
    CHECK(plotted.Plot);
    CHECK(plotted.Rgb == 0xff0000);
    CHECK(Written(plotted).find("290") == std::string::npos);
  }

  TEST_CASE("layer: a book color stands after its index, as AutoCAD writes it")
  {
    constexpr std::string_view booked =
        "  0\r\nLAYER\r\n  5\r\n10\r\n330\r\n2\r\n100\r\n"
        "AcDbSymbolTableRecord\r\n100\r\nAcDbLayerTableRecord\r\n"
        "  2\r\nBooked\r\n 70\r\n     0\r\n 62\r\n   254\r\n"
        "420\r\n 16305843\r\n430\r\nDIC COLOR GUIDE(R)$DIC 1\r\n"
        "  6\r\nContinuous\r\n370\r\n    -3\r\n390\r\nF\r\n347\r\n2A\r\n"
        "348\r\n0\r\n";
    const Layer taken = Read<Layer>(booked);
    CHECK(taken.Rgb  == 16305843);
    CHECK(taken.Book == "DIC COLOR GUIDE(R)$DIC 1");
    CHECK(taken.Record.Base.Extra.empty());
    CHECK(Written(taken) == booked);
  }

  TEST_CASE("layer: a null plot style keeps its 390 from R2000, as AutoCAD needs one")
  {
    constexpr std::string_view unstyled =
        "  0\r\nLAYER\r\n  5\r\n10\r\n330\r\n2\r\n100\r\n"
        "AcDbSymbolTableRecord\r\n100\r\nAcDbLayerTableRecord\r\n"
        "  2\r\n0\r\n 70\r\n     0\r\n 62\r\n     7\r\n"
        "  6\r\nContinuous\r\n370\r\n    -3\r\n390\r\n0\r\n347\r\n2A\r\n"
        "348\r\n0\r\n";
    const Layer taken = Read<Layer>(unstyled);
    CHECK_FALSE(taken.Style);
    CHECK(Written(taken) == unstyled);
    CHECK(Written(Layer{}, Version::R2000).contains("390\r\n0\r\n"));
    CHECK_FALSE(Written(Layer{}, Version::R14).contains("390"));
  }

  TEST_CASE("layer: its linetype named by a symbol, as an entity's is")
  {
    const Layer fresh{};
    CHECK(fresh.Linetype             == Symbol{"Continuous"});
    CHECK(Read<Layer>(Text).Linetype == Symbol{"DASHED"});
  }

  TEST_CASE("layer: before R2000 no lineweight, plot style or marker")
  {
    const Layer old =
        Read<Layer>("  0\nLAYER\n  2\n0\n 70\n0\n 62\n7\n  6\nCONTINUOUS\n", Version::R12);
    CHECK(old.Linetype == "CONTINUOUS");
    CHECK(Written(old, Version::R12)
          == "  0\r\nLAYER\r\n  2\r\n0\r\n 70\r\n     0\r\n 62\r\n     7\r\n"
             "  6\r\nCONTINUOUS\r\n");
  }
} // namespace DxfIO::Tables
