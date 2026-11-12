#include "objects/shapes/mline/style.hpp"

#include "../../kind.hpp"

#include <string_view>

namespace DxfIO::Objects::Mline
{
  TEST_CASE("style: AutoCAD's Standard read whole, its two lines")
  {
    constexpr std::string_view text  = "  0\r\nMLINESTYLE\r\n"
                                       "  5\r\n18\r\n"
                                       "102\r\n{ACAD_REACTORS\r\n"
                                       "330\r\n17\r\n"
                                       "102\r\n}\r\n"
                                       "330\r\n17\r\n"
                                       "100\r\nAcDbMlineStyle\r\n"
                                       "  2\r\nSTANDARD\r\n"
                                       " 70\r\n     0\r\n"
                                       "  3\r\n\r\n"
                                       " 62\r\n   256\r\n"
                                       " 51\r\n90.0\r\n"
                                       " 52\r\n90.0\r\n"
                                       " 71\r\n     2\r\n"
                                       " 49\r\n0.5\r\n"
                                       " 62\r\n   256\r\n"
                                       "  6\r\nBYLAYER\r\n"
                                       " 49\r\n-0.5\r\n"
                                       " 62\r\n   256\r\n"
                                       "  6\r\nBYLAYER\r\n";
    const MLineStyle           taken = Read<MLineStyle>(text);
    CHECK(taken.Name == "STANDARD");
    REQUIRE(taken.Elements.size()      == 2);
    CHECK(taken.Elements.back().Offset == -0.5);
    CHECK(taken.Base.Extra.empty());
    CHECK(Written(taken) == text);
  }

  TEST_CASE("style: a line given no color is ByLayer, the next keeps its own")
  {
    constexpr std::string_view text  = "  0\r\nMLINESTYLE\r\n"
                                       "  5\r\n18\r\n"
                                       "330\r\n17\r\n"
                                       "100\r\nAcDbMlineStyle\r\n"
                                       "  2\r\nWALL\r\n"
                                       " 70\r\n     1\r\n"
                                       "  3\r\n\r\n"
                                       " 62\r\n     8\r\n"
                                       " 51\r\n90.0\r\n"
                                       " 52\r\n90.0\r\n"
                                       " 71\r\n     2\r\n"
                                       " 49\r\n1.0\r\n"
                                       "  6\r\nBYLAYER\r\n"
                                       " 49\r\n-1.0\r\n"
                                       " 62\r\n     1\r\n"
                                       "  6\r\nBYLAYER\r\n";
    const MLineStyle           taken = Read<MLineStyle>(text);
    CHECK(taken.Fill == 8);
    REQUIRE(taken.Elements.size()      == 2);
    CHECK(taken.Elements.front().Color == Colors::Bylayer);
    CHECK(taken.Elements.back().Color  == 1);
  }

  TEST_CASE("style: a count past its lines makes none from nothing")
  {
    constexpr std::string_view text  = "  0\r\nMLINESTYLE\r\n"
                                       "  5\r\n18\r\n"
                                       "330\r\n17\r\n"
                                       "100\r\nAcDbMlineStyle\r\n"
                                       "  2\r\nSTANDARD\r\n"
                                       " 71\r\n     3\r\n"
                                       " 49\r\n0.5\r\n";
    const MLineStyle           taken = Read<MLineStyle>(text);
    REQUIRE(taken.Elements.size()       == 1);
    CHECK(taken.Elements.front().Offset == 0.5);
  }
} // namespace DxfIO::Objects::Mline
