#include "objects/shapes/layout.hpp"

#include "../entities/drawn.hpp"
#include "../kind.hpp"

#include <string>
#include <string_view>

namespace DxfIO::Objects
{
TEST_CASE("layout: the model's read from AutoCAD's record, its last viewport")
{
  constexpr std::string_view text = "  0\r\nLAYOUT\r\n"
                                    "  5\r\n22\r\n"
                                    "102\r\n{ACAD_XDICTIONARY\r\n"
                                    "360\r\n205\r\n"
                                    "102\r\n}\r\n"
                                    "102\r\n{ACAD_REACTORS\r\n"
                                    "330\r\n1A\r\n"
                                    "102\r\n}\r\n"
                                    "330\r\n1A\r\n"
                                    "100\r\nAcDbPlotSettings\r\n"
                                    "  1\r\n\r\n"
                                    "  2\r\nnone_device\r\n"
                                    "  4\r\nISO_A4_(210.00_x_297.00_MM)\r\n"
                                    "  6\r\n\r\n"
                                    " 40\r\n7.5\r\n"
                                    " 41\r\n20.0\r\n"
                                    " 42\r\n7.5\r\n"
                                    " 43\r\n20.0\r\n"
                                    " 44\r\n210.0\r\n"
                                    " 45\r\n297.0\r\n"
                                    " 46\r\n11.54999923706054\r\n"
                                    " 47\r\n-13.65000009536743\r\n"
                                    " 48\r\n0.0\r\n"
                                    " 49\r\n0.0\r\n"
                                    "140\r\n0.0\r\n"
                                    "141\r\n0.0\r\n"
                                    "142\r\n1.0\r\n"
                                    "143\r\n8.704084754739808\r\n"
                                    " 70\r\n 11952\r\n"
                                    " 72\r\n     1\r\n"
                                    " 73\r\n     0\r\n"
                                    " 74\r\n     0\r\n"
                                    "  7\r\n\r\n"
                                    " 75\r\n     0\r\n"
                                    "147\r\n0.1148885871608098\r\n"
                                    " 76\r\n     0\r\n"
                                    " 77\r\n     2\r\n"
                                    " 78\r\n   300\r\n"
                                    "148\r\n0.0\r\n"
                                    "149\r\n0.0\r\n"
                                    "100\r\nAcDbLayout\r\n"
                                    "  1\r\nModel\r\n"
                                    " 70\r\n     1\r\n"
                                    " 71\r\n     0\r\n"
                                    " 10\r\n0.0\r\n"
                                    " 20\r\n0.0\r\n"
                                    " 11\r\n12.0\r\n"
                                    " 21\r\n9.0\r\n"
                                    " 12\r\n0.0\r\n"
                                    " 22\r\n0.0\r\n"
                                    " 32\r\n0.0\r\n"
                                    " 14\r\n0.0\r\n"
                                    " 24\r\n0.0\r\n"
                                    " 34\r\n0.0\r\n"
                                    " 15\r\n0.0\r\n"
                                    " 25\r\n0.0\r\n"
                                    " 35\r\n0.0\r\n"
                                    "146\r\n0.0\r\n"
                                    " 13\r\n0.0\r\n"
                                    " 23\r\n0.0\r\n"
                                    " 33\r\n0.0\r\n"
                                    " 16\r\n1.0\r\n"
                                    " 26\r\n0.0\r\n"
                                    " 36\r\n0.0\r\n"
                                    " 17\r\n0.0\r\n"
                                    " 27\r\n1.0\r\n"
                                    " 37\r\n0.0\r\n"
                                    " 76\r\n     0\r\n"
                                    "330\r\n1F\r\n"
                                    "331\r\nEA\r\n"
                                    "1001\r\nACAD_PSEXT\r\n"
                                    "1000\r\nNone\r\n"
                                    "1000\r\nNone\r\n"
                                    "1000\r\nNot applicable\r\n"
                                    "1000\r\nThe layout will not be plotted unless a new plotter "
                                    "configuration name is selected.\r\n"
                                    "1070\r\n     0\r\n";
  const Layout taken = Read<Layout>(text);
  CHECK(taken.Name == "Model");
  CHECK(taken.Order == 0);
  CHECK(taken.Plot->Size == "ISO_A4_(210.00_x_297.00_MM)");
  CHECK(taken.Plot->Origin.Y == -13.65000009536743);
  CHECK(taken.Plot->Drawing == 8.704084754739808);
  CHECK(taken.Plot->Flags == 11952);
  CHECK(taken.Block == Handle{.Value = 0x1f});
  CHECK(taken.Viewport == Handle{.Value = 0xea});
  CHECK(taken.Base.Owner == Handle{.Value = 0x1a});
  CHECK(taken.Base.Extended.size() == 1);
  CHECK(Written(taken) == text);
}

TEST_CASE("layout: R2000's paper layout, no shade plot, at AutoCAD's defaults")
{
  constexpr std::string_view text = "  0\r\nLAYOUT\r\n"
                                    "  5\r\nD3\r\n"
                                    "102\r\n{ACAD_XDICTIONARY\r\n"
                                    "360\r\n3AE\r\n"
                                    "102\r\n}\r\n"
                                    "102\r\n{ACAD_REACTORS\r\n"
                                    "330\r\n1A\r\n"
                                    "102\r\n}\r\n"
                                    "330\r\n1A\r\n"
                                    "100\r\nAcDbPlotSettings\r\n"
                                    "  1\r\n\r\n"
                                    "  2\r\nDefault Windows System Printer.pc3\r\n"
                                    "  4\r\n\r\n"
                                    "  6\r\n\r\n"
                                    " 40\r\n0.0\r\n"
                                    " 41\r\n0.0\r\n"
                                    " 42\r\n0.0\r\n"
                                    " 43\r\n0.0\r\n"
                                    " 44\r\n0.0\r\n"
                                    " 45\r\n0.0\r\n"
                                    " 46\r\n0.0\r\n"
                                    " 47\r\n0.0\r\n"
                                    " 48\r\n0.0\r\n"
                                    " 49\r\n0.0\r\n"
                                    "140\r\n0.0\r\n"
                                    "141\r\n0.0\r\n"
                                    "142\r\n1.0\r\n"
                                    "143\r\n1.0\r\n"
                                    " 70\r\n   688\r\n"
                                    " 72\r\n     0\r\n"
                                    " 73\r\n     0\r\n"
                                    " 74\r\n     5\r\n"
                                    "  7\r\n\r\n"
                                    " 75\r\n    16\r\n"
                                    "147\r\n1.0\r\n"
                                    "148\r\n0.0\r\n"
                                    "149\r\n0.0\r\n"
                                    "100\r\nAcDbLayout\r\n"
                                    "  1\r\nLayout1\r\n"
                                    " 70\r\n     1\r\n"
                                    " 71\r\n     1\r\n"
                                    " 10\r\n0.0\r\n"
                                    " 20\r\n0.0\r\n"
                                    " 11\r\n12.0\r\n"
                                    " 21\r\n9.0\r\n"
                                    " 12\r\n0.0\r\n"
                                    " 22\r\n0.0\r\n"
                                    " 32\r\n0.0\r\n"
                                    " 14\r\n0.0\r\n"
                                    " 24\r\n0.0\r\n"
                                    " 34\r\n0.0\r\n"
                                    " 15\r\n0.0\r\n"
                                    " 25\r\n0.0\r\n"
                                    " 35\r\n0.0\r\n"
                                    "146\r\n0.0\r\n"
                                    " 13\r\n0.0\r\n"
                                    " 23\r\n0.0\r\n"
                                    " 33\r\n0.0\r\n"
                                    " 16\r\n1.0\r\n"
                                    " 26\r\n0.0\r\n"
                                    " 36\r\n0.0\r\n"
                                    " 17\r\n0.0\r\n"
                                    " 27\r\n1.0\r\n"
                                    " 37\r\n0.0\r\n"
                                    " 76\r\n     0\r\n"
                                    "330\r\nD2\r\n";
  const Layout taken = Read<Layout>(text, Version::R2000);
  CHECK(taken.Name == "Layout1");
  CHECK(taken.Order == 1);
  Plot fresh{};
  fresh.Device = "Default Windows System Printer.pc3";
  CHECK(*taken.Plot == fresh);
  CHECK(taken.Limmax == Limits);
  CHECK(taken.Axes == Tables::Axes{});
  CHECK_FALSE(taken.Viewport);
  CHECK(Written(taken, Version::R2000) == text);
}

TEST_CASE("layout: a named UCS between the extents and the elevation, as AutoCAD writes it")
{
  const std::string text = Entities::Drawn("D7", Version::R2018, "layouts");
  const Layout taken = Read<Layout>(text);
  CHECK(taken.Name == "Layout2");
  CHECK(taken.Axes.Named == Handle{.Value = 0x2f1});
  CHECK(taken.Base.Extra.empty());
  CHECK(Written(taken) == text);
  Layout based = taken;
  based.Axes.Basis = Handle{.Value = 0x2f2};
  const std::string written = Written(based);
  CHECK(written.contains("345\r\n2F1\r\n346\r\n2F2\r\n146\r\n"));
}
} // namespace DxfIO::Objects
