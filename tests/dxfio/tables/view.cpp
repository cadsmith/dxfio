#include "tables/shapes/view.hpp"

#include "../kind.hpp"

#include <string_view>

namespace DxfIO::Tables
{
  TEST_CASE("view: a view with a named UCS read from AutoCAD's record")
  {
    constexpr std::string_view text  = "  0\r\nVIEW\r\n"
                                       "  5\r\n2D6\r\n"
                                       "102\r\n{ACAD_XDICTIONARY\r\n"
                                       "360\r\n2D7\r\n"
                                       "102\r\n}\r\n"
                                       "330\r\n6\r\n"
                                       "100\r\nAcDbSymbolTableRecord\r\n"
                                       "100\r\nAcDbViewTableRecord\r\n"
                                       "  2\r\nturned\r\n"
                                       " 70\r\n     0\r\n"
                                       " 40\r\n2653.730076586097\r\n"
                                       " 10\r\n2630.048781668382\r\n"
                                       " 20\r\n1440.684066268786\r\n"
                                       " 41\r\n4685.103193372453\r\n"
                                       " 11\r\n0.0\r\n"
                                       " 21\r\n0.0\r\n"
                                       " 31\r\n1.0\r\n"
                                       " 12\r\n0.0\r\n"
                                       " 22\r\n0.0\r\n"
                                       " 32\r\n0.0\r\n"
                                       " 42\r\n50.0\r\n"
                                       " 43\r\n0.0\r\n"
                                       " 44\r\n0.0\r\n"
                                       " 50\r\n0.0\r\n"
                                       " 71\r\n     0\r\n"
                                       "281\r\n     0\r\n"
                                       " 72\r\n     1\r\n"
                                       "110\r\n0.0\r\n"
                                       "120\r\n0.0\r\n"
                                       "130\r\n0.0\r\n"
                                       "111\r\n0.8660254037844387\r\n"
                                       "121\r\n0.5\r\n"
                                       "131\r\n0.0\r\n"
                                       "112\r\n-0.5\r\n"
                                       "122\r\n0.8660254037844387\r\n"
                                       "132\r\n0.0\r\n"
                                       "345\r\n2D5\r\n"
                                       " 79\r\n     0\r\n"
                                       "146\r\n0.0\r\n"
                                       " 73\r\n     0\r\n"
                                       "348\r\nF5\r\n";
    const View                 taken = Read<View>(text);
    CHECK(taken.Record.Name == "turned");
    CHECK(taken.Associated);
    CHECK(taken.Axes.Named == Handle{.Value = 0x2D5});
    CHECK(taken.Axes.Y.X   == -0.5);
    CHECK(taken.Style      == Handle{.Value = 0xF5});
    CHECK(taken.Record.Base.Dictionary == Handle{.Value = 0x2D7});
    CHECK(Written(taken) == text);
  }

  TEST_CASE("view: R2000 has no camera nor styles")
  {
    constexpr std::string_view text  = "  0\r\nVIEW\r\n"
                                       "  5\r\n2D2\r\n"
                                       "102\r\n{ACAD_XDICTIONARY\r\n"
                                       "360\r\n2D3\r\n"
                                       "102\r\n}\r\n"
                                       "330\r\n6\r\n"
                                       "100\r\nAcDbSymbolTableRecord\r\n"
                                       "100\r\nAcDbViewTableRecord\r\n"
                                       "  2\r\nplain\r\n"
                                       " 70\r\n     0\r\n"
                                       " 40\r\n2653.730076586097\r\n"
                                       " 10\r\n2630.048781668382\r\n"
                                       " 20\r\n1440.684066268786\r\n"
                                       " 41\r\n4685.103193372453\r\n"
                                       " 11\r\n0.0\r\n"
                                       " 21\r\n0.0\r\n"
                                       " 31\r\n1.0\r\n"
                                       " 12\r\n0.0\r\n"
                                       " 22\r\n0.0\r\n"
                                       " 32\r\n0.0\r\n"
                                       " 42\r\n50.0\r\n"
                                       " 43\r\n0.0\r\n"
                                       " 44\r\n0.0\r\n"
                                       " 50\r\n0.0\r\n"
                                       " 71\r\n     0\r\n"
                                       "281\r\n     0\r\n"
                                       " 72\r\n     1\r\n"
                                       "110\r\n0.0\r\n"
                                       "120\r\n0.0\r\n"
                                       "130\r\n0.0\r\n"
                                       "111\r\n1.0\r\n"
                                       "121\r\n0.0\r\n"
                                       "131\r\n0.0\r\n"
                                       "112\r\n0.0\r\n"
                                       "122\r\n1.0\r\n"
                                       "132\r\n0.0\r\n"
                                       " 79\r\n     0\r\n"
                                       "146\r\n0.0\r\n";
    const View                 taken = Read<View>(text, Version::R2000);
    CHECK(taken.Axes  == Axes{});
    CHECK(taken.Style == Handle{});
    CHECK(Written(taken, Version::R2000) == text);
  }

  TEST_CASE("view: R12 has no render mode nor UCS")
  {
    constexpr std::string_view text  = "  0\r\nVIEW\r\n  2\r\nTURNED\r\n 70\r\n     0\r\n"
                                       " 40\r\n2653.730076586097\r\n 10\r\n2630.048781668382\r\n"
                                       " 20\r\n1440.684066268786\r\n 41\r\n4685.103193372453\r\n"
                                       " 11\r\n0.0\r\n 21\r\n0.0\r\n 31\r\n1.0\r\n"
                                       " 12\r\n0.0\r\n 22\r\n0.0\r\n 32\r\n0.0\r\n"
                                       " 42\r\n50.0\r\n 43\r\n0.0\r\n 44\r\n0.0\r\n 50\r\n0.0\r\n"
                                       " 71\r\n     0\r\n";
    View                       taken = Read<View>(text, Version::R12);
    CHECK(taken.Height == 2653.730076586097);
    CHECK_FALSE(taken.Associated);
    taken.Associated = true;
    CHECK(Written(taken, Version::R12) == text);
  }
} // namespace DxfIO::Tables
