#include "tables/shapes/linetype.hpp"

#include "../kind.hpp"

#include <string_view>

namespace DxfIO::Tables
{
TEST_CASE("linetype: a text on the line read from AutoCAD's record")
{
  constexpr std::string_view text = "  0\r\nLTYPE\r\n"
                                    "  5\r\n2CD\r\n"
                                    "330\r\n5\r\n"
                                    "100\r\nAcDbSymbolTableRecord\r\n"
                                    "100\r\nAcDbLinetypeTableRecord\r\n"
                                    "  2\r\nGAS_LINE\r\n"
                                    " 70\r\n     0\r\n"
                                    "  3\r\nGas line ----GAS----GAS----GAS----GAS----GAS---\r\n"
                                    " 72\r\n    65\r\n"
                                    " 73\r\n     3\r\n"
                                    " 40\r\n24.13\r\n"
                                    " 49\r\n12.7\r\n"
                                    " 74\r\n     0\r\n"
                                    " 49\r\n-5.08\r\n"
                                    " 74\r\n    10\r\n"
                                    " 75\r\n     0\r\n"
                                    "340\r\n11\r\n"
                                    " 46\r\n2.54\r\n"
                                    " 50\r\n0.0\r\n"
                                    " 44\r\n-2.54\r\n"
                                    " 45\r\n-1.27\r\n"
                                    "  9\r\nGAS\r\n"
                                    " 49\r\n-6.349999999999999\r\n"
                                    " 74\r\n     0\r\n";
  const Ltype taken = Read<Ltype>(text);
  CHECK(taken.Record.Name == "GAS_LINE");
  REQUIRE(taken.Dashes.size() == 3);
  CHECK(taken.Dashes.at(1).Text == "GAS");
  CHECK(taken.Dashes.at(1).Style == Handle{.Value = 0x11});
  CHECK(taken.Dashes.at(1).Across == -1.27);
  CHECK(taken.Dashes.at(2).Length == -6.349999999999999);
  CHECK(Written(taken) == text);
}

TEST_CASE("linetype: shapes on the line read from AutoCAD's record")
{
  constexpr std::string_view text = "  0\r\nLTYPE\r\n"
                                    "  5\r\n2D0\r\n"
                                    "330\r\n5\r\n"
                                    "100\r\nAcDbSymbolTableRecord\r\n"
                                    "100\r\nAcDbLinetypeTableRecord\r\n"
                                    "  2\r\nZIGZAG\r\n"
                                    " 70\r\n     0\r\n"
                                    "  3\r\nZig zag "
                                    "/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\r\n"
                                    " 72\r\n    65\r\n"
                                    " 73\r\n     4\r\n"
                                    " 40\r\n20.32254\r\n"
                                    " 49\r\n0.00254\r\n"
                                    " 74\r\n     0\r\n"
                                    " 49\r\n-5.08\r\n"
                                    " 74\r\n     4\r\n"
                                    " 75\r\n   131\r\n"
                                    "340\r\n2CF\r\n"
                                    " 46\r\n5.08\r\n"
                                    " 50\r\n0.0\r\n"
                                    " 44\r\n-5.08\r\n"
                                    " 45\r\n0.0\r\n"
                                    " 49\r\n-10.16\r\n"
                                    " 74\r\n     4\r\n"
                                    " 75\r\n   131\r\n"
                                    "340\r\n2CF\r\n"
                                    " 46\r\n5.08\r\n"
                                    " 50\r\n180.0\r\n"
                                    " 44\r\n5.08\r\n"
                                    " 45\r\n0.0\r\n"
                                    " 49\r\n-5.08\r\n"
                                    " 74\r\n     0\r\n";
  const Ltype taken = Read<Ltype>(text);
  REQUIRE(taken.Dashes.size() == 4);
  CHECK(taken.Dashes.at(2).Shape == 131);
  CHECK(taken.Dashes.at(2).Rotation == 180.0);
  CHECK(taken.Dashes.at(2).Text.empty());
  CHECK(Written(taken) == text);
}

TEST_CASE("linetype: R12 keeps only the lengths of the dashes")
{
  constexpr std::string_view text = "  0\r\nLTYPE\r\n  2\r\nGAS_LINE\r\n 70\r\n     0\r\n"
                                    "  3\r\nGas line ----GAS----GAS----GAS----GAS----GAS---\r\n"
                                    " 72\r\n    65\r\n 73\r\n     3\r\n 40\r\n24.13\r\n"
                                    " 49\r\n12.7\r\n 49\r\n-5.08\r\n 49\r\n-6.349999999999999\r\n";
  const Ltype taken = Read<Ltype>(text, Version::R12);
  REQUIRE(taken.Dashes.size() == 3);
  CHECK(taken.Dashes.at(1).Flags == 0);
  CHECK(Written(taken, Version::R12) == text);
  Ltype later = taken;
  later.Dashes.at(1) = Dash{.Length = -5.08, .Flags = 2, .Text = "GAS"};
  CHECK(Written(later, Version::R12) == text);
}
} // namespace DxfIO::Tables
