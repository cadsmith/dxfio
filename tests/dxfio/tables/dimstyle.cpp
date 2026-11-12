#include "tables/shapes/dimstyle.hpp"

#include "../kind.hpp"

#include <string>
#include <string_view>

namespace DxfIO::Tables
{
  TEST_CASE("dimstyle: a style off its defaults read from AutoCAD's record")
  {
    constexpr std::string_view text  = "  0\r\nDIMSTYLE\r\n"
                                       "105\r\n2ED\r\n"
                                       "330\r\nA\r\n"
                                       "100\r\nAcDbSymbolTableRecord\r\n"
                                       "100\r\nAcDbDimStyleTableRecord\r\n"
                                       "  2\r\nfull\r\n"
                                       " 70\r\n     0\r\n"
                                       "  3\r\nmm\r\n"
                                       "  4\r\nin\r\n"
                                       " 40\r\n2.0\r\n"
                                       " 41\r\n3.0\r\n"
                                       " 42\r\n0.625\r\n"
                                       " 43\r\n3.75\r\n"
                                       " 44\r\n1.25\r\n"
                                       " 49\r\n5.0\r\n"
                                       " 50\r\n30.0\r\n"
                                       " 69\r\n     2\r\n"
                                       " 70\r\n     5\r\n"
                                       " 71\r\n     1\r\n"
                                       " 73\r\n     0\r\n"
                                       " 74\r\n     0\r\n"
                                       " 77\r\n     1\r\n"
                                       " 78\r\n     8\r\n"
                                       " 79\r\n     3\r\n"
                                       " 90\r\n        1\r\n"
                                       "140\r\n3.0\r\n"
                                       "141\r\n2.5\r\n"
                                       "143\r\n0.03937007874016\r\n"
                                       "147\r\n0.625\r\n"
                                       "148\r\n0.5\r\n"
                                       "170\r\n     1\r\n"
                                       "171\r\n     3\r\n"
                                       "172\r\n     1\r\n"
                                       "173\r\n     1\r\n"
                                       "176\r\n     1\r\n"
                                       "177\r\n     2\r\n"
                                       "178\r\n     3\r\n"
                                       "271\r\n     2\r\n"
                                       "272\r\n     3\r\n"
                                       "274\r\n     3\r\n"
                                       "278\r\n    44\r\n"
                                       "279\r\n     1\r\n"
                                       "283\r\n     0\r\n"
                                       "284\r\n     8\r\n"
                                       "289\r\n     1\r\n"
                                       "290\r\n     1\r\n"
                                       "340\r\n11\r\n"
                                       "341\r\n2E9\r\n"
                                       "343\r\n2DF\r\n"
                                       "344\r\n2E5\r\n"
                                       "345\r\n2CC\r\n"
                                       "346\r\n2CC\r\n"
                                       "347\r\n2CC\r\n"
                                       "371\r\n    30\r\n"
                                       "372\r\n    50\r\n";
    const Dimstyle             taken = Read<Dimstyle>(text);
    CHECK(taken.Record.Name      == "full");
    CHECK(taken.Record.Base.Self == Handle{.Value = 0x2ED});
    CHECK(taken.Dimpost          == "mm");
    CHECK(taken.Dimscale         == 2.0);
    CHECK(taken.Dimjogang        == 30.0);
    CHECK(taken.Dimtfill         == 2);
    CHECK(taken.Dimtfillclr      == 5);
    CHECK(taken.Dimfxlon);
    CHECK(taken.Dimltype == Handle{.Value = 0x2CC});
    CHECK(taken.Dimexo   == 0.625);
    CHECK(taken.Dimtsz   == 0.0);
    CHECK(Written(taken) == text);
  }

  TEST_CASE("dimstyle: a style at its defaults gives only its text style")
  {
    constexpr std::string_view text  = "  0\r\nDIMSTYLE\r\n"
                                       "105\r\n1B0\r\n"
                                       "330\r\nA\r\n"
                                       "100\r\nAcDbSymbolTableRecord\r\n"
                                       "100\r\nAcDbDimStyleTableRecord\r\n"
                                       "  2\r\nStandard\r\n"
                                       " 70\r\n     0\r\n"
                                       "340\r\n11\r\n";
    const Dimstyle             taken = Read<Dimstyle>(text);
    CHECK(taken.Dimasz    == Size);
    CHECK(taken.Dimdsep   == '.');
    CHECK(taken.Dimjogang == Jog);
    CHECK(Written(taken)  == text);
  }

  TEST_CASE("dimstyle: R2000 keeps R2007's variables in extended data")
  {
    constexpr std::string_view text  = "  0\r\nDIMSTYLE\r\n"
                                       "105\r\n2ED\r\n"
                                       "330\r\nA\r\n"
                                       "100\r\nAcDbSymbolTableRecord\r\n"
                                       "100\r\nAcDbDimStyleTableRecord\r\n"
                                       "  2\r\nfull\r\n"
                                       " 70\r\n     0\r\n"
                                       "  3\r\nmm\r\n"
                                       "  4\r\nin\r\n"
                                       " 40\r\n2.0\r\n"
                                       " 41\r\n3.0\r\n"
                                       " 42\r\n0.625\r\n"
                                       " 43\r\n3.75\r\n"
                                       " 44\r\n1.25\r\n"
                                       " 71\r\n     1\r\n"
                                       " 73\r\n     0\r\n"
                                       " 74\r\n     0\r\n"
                                       " 77\r\n     1\r\n"
                                       " 78\r\n     8\r\n"
                                       " 79\r\n     3\r\n"
                                       "140\r\n3.0\r\n"
                                       "141\r\n2.5\r\n"
                                       "143\r\n0.03937007874016\r\n"
                                       "147\r\n0.625\r\n"
                                       "148\r\n0.5\r\n"
                                       "170\r\n     1\r\n"
                                       "171\r\n     3\r\n"
                                       "172\r\n     1\r\n"
                                       "173\r\n     1\r\n"
                                       "176\r\n     1\r\n"
                                       "177\r\n     2\r\n"
                                       "178\r\n     3\r\n"
                                       "271\r\n     2\r\n"
                                       "272\r\n     3\r\n"
                                       "274\r\n     3\r\n"
                                       "278\r\n    44\r\n"
                                       "279\r\n     1\r\n"
                                       "283\r\n     0\r\n"
                                       "284\r\n     8\r\n"
                                       "289\r\n     1\r\n"
                                       "340\r\n11\r\n"
                                       "341\r\n2E9\r\n"
                                       "343\r\n2DF\r\n"
                                       "344\r\n2E5\r\n"
                                       "371\r\n    30\r\n"
                                       "372\r\n    50\r\n"
                                       "1001\r\nACAD_DSTYLE_DIMARC_LENGTH_SYMBOL\r\n"
                                       "1070\r\n   379\r\n"
                                       "1070\r\n     1\r\n"
                                       "1001\r\nACAD_DSTYLE_DIMEXT_ENABLED\r\n"
                                       "1070\r\n   383\r\n"
                                       "1070\r\n     1\r\n"
                                       "1001\r\nACAD_DSTYLE_DIMEXT_LENGTH\r\n"
                                       "1070\r\n   378\r\n"
                                       "1040\r\n5.0\r\n"
                                       "1001\r\nACAD_DSTYLE_DIMJOGGED_JOGANGLE_SYMBOL\r\n"
                                       "1070\r\n   384\r\n"
                                       "1040\r\n0.5235987755982988\r\n"
                                       "1001\r\nACAD_DSTYLE_DIMTEXT_FILL\r\n"
                                       "1070\r\n   376\r\n"
                                       "1070\r\n     2\r\n"
                                       "1070\r\n   377\r\n"
                                       "1004\r\n1800000001000000050000C3000000000000000000000000\r\n"
                                       "1001\r\nACAD_DSTYLE_DIM_LINETYPE\r\n"
                                       "1070\r\n   380\r\n"
                                       "1005\r\n2CC\r\n"
                                       "1001\r\nACAD_DSTYLE_DIM_EXT1_LINETYPE\r\n"
                                       "1070\r\n   381\r\n"
                                       "1005\r\n2CC\r\n"
                                       "1001\r\nACAD_DSTYLE_DIM_EXT2_LINETYPE\r\n"
                                       "1070\r\n   382\r\n"
                                       "1005\r\n2CC\r\n";
    const Dimstyle             taken = Read<Dimstyle>(text, Version::R2000);
    CHECK(taken.Dimtfill == 0);
    CHECK(taken.Record.Base.Extended.size() == 8);
    CHECK(Written(taken, Version::R2000)    == text);
  }

  TEST_CASE("dimstyle: R12 gives every variable and the arrowheads by name")
  {
    constexpr std::string_view text  = "  0\r\nDIMSTYLE\r\n"
                                       "  2\r\nFULL\r\n"
                                       " 70\r\n     0\r\n"
                                       "  3\r\nmm\r\n"
                                       "  4\r\nin\r\n"
                                       "  5\r\n\r\n"
                                       "  6\r\nOPEN\r\n"
                                       "  7\r\nOBLIQUE\r\n"
                                       " 40\r\n2.0\r\n"
                                       " 41\r\n3.0\r\n"
                                       " 42\r\n0.625\r\n"
                                       " 43\r\n3.75\r\n"
                                       " 44\r\n1.25\r\n"
                                       " 45\r\n0.0\r\n"
                                       " 46\r\n0.0\r\n"
                                       " 47\r\n0.0\r\n"
                                       " 48\r\n0.0\r\n"
                                       "140\r\n3.0\r\n"
                                       "141\r\n2.5\r\n"
                                       "142\r\n0.0\r\n"
                                       "143\r\n0.03937007874016\r\n"
                                       "144\r\n1.0\r\n"
                                       "145\r\n0.0\r\n"
                                       "146\r\n1.0\r\n"
                                       "147\r\n0.625\r\n"
                                       " 71\r\n     1\r\n"
                                       " 72\r\n     0\r\n"
                                       " 73\r\n     0\r\n"
                                       " 74\r\n     0\r\n"
                                       " 75\r\n     0\r\n"
                                       " 76\r\n     0\r\n"
                                       " 77\r\n     1\r\n"
                                       " 78\r\n     8\r\n"
                                       "170\r\n     1\r\n"
                                       "171\r\n     3\r\n"
                                       "172\r\n     1\r\n"
                                       "173\r\n     1\r\n"
                                       "174\r\n     0\r\n"
                                       "175\r\n     0\r\n"
                                       "176\r\n     1\r\n"
                                       "177\r\n     2\r\n"
                                       "178\r\n     3\r\n";
    const Dimstyle             taken = Read<Dimstyle>(text, Version::R12);
    CHECK(taken.Arrows.Dimblk.empty());
    CHECK(taken.Arrows.Dimblk1 == "OPEN");
    CHECK(taken.Dimclrt        == 3);
    CHECK(Written(taken, Version::R12) == text);
  }

  TEST_CASE("dimstyle: written before R2000, every variable, at R12's place")
  {
    const std::string text = Written(Dimstyle{}, Version::R12);
    CHECK(text.find(" 48\r\n0.0\r\n140\r\n0.18\r\n") != std::string::npos);
    CHECK(text.find("147\r\n0.09\r\n 71\r\n") != std::string::npos);
    CHECK(text.ends_with("178\r\n     0\r\n"));
    CHECK(Written(Dimstyle{}).ends_with(" 70\r\n     0\r\n"));
  }
} // namespace DxfIO::Tables
