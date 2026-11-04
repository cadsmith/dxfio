#include "objects/shapes/mleader/style.hpp"

#include "../../kind.hpp"

#include <string_view>

namespace DxfIO::Objects::Mleader
{
TEST_CASE("style: AutoCAD's Standard read whole from R2018")
{
  constexpr std::string_view text = "  0\r\nMLEADERSTYLE\r\n"
                                    "  5\r\n12E\r\n"
                                    "102\r\n{ACAD_REACTORS\r\n"
                                    "330\r\n12D\r\n"
                                    "102\r\n}\r\n"
                                    "330\r\n12D\r\n"
                                    "100\r\nAcDbMLeaderStyle\r\n"
                                    "179\r\n     2\r\n"
                                    "170\r\n     2\r\n"
                                    "171\r\n     1\r\n"
                                    "172\r\n     0\r\n"
                                    " 90\r\n        2\r\n"
                                    " 40\r\n0.0\r\n"
                                    " 41\r\n0.0\r\n"
                                    "173\r\n     1\r\n"
                                    " 91\r\n-1056964608\r\n"
                                    "340\r\n14\r\n"
                                    " 92\r\n       -2\r\n"
                                    "290\r\n     1\r\n"
                                    " 42\r\n2.0\r\n"
                                    "291\r\n     1\r\n"
                                    " 43\r\n8.0\r\n"
                                    "  3\r\nStandard\r\n"
                                    " 44\r\n4.0\r\n"
                                    "300\r\n\r\n"
                                    "342\r\n11\r\n"
                                    "174\r\n     1\r\n"
                                    "178\r\n     1\r\n"
                                    "175\r\n     1\r\n"
                                    "176\r\n     0\r\n"
                                    " 93\r\n-1056964608\r\n"
                                    " 45\r\n4.0\r\n"
                                    "292\r\n     0\r\n"
                                    "297\r\n     0\r\n"
                                    " 46\r\n4.0\r\n"
                                    " 94\r\n-1056964608\r\n"
                                    " 47\r\n1.0\r\n"
                                    " 49\r\n1.0\r\n"
                                    "140\r\n1.0\r\n"
                                    "293\r\n     1\r\n"
                                    "141\r\n0.0\r\n"
                                    "294\r\n     1\r\n"
                                    "177\r\n     0\r\n"
                                    "142\r\n1.0\r\n"
                                    "295\r\n     0\r\n"
                                    "296\r\n     0\r\n"
                                    "143\r\n3.75\r\n"
                                    "271\r\n     0\r\n"
                                    "272\r\n     9\r\n"
                                    "273\r\n     9\r\n"
                                    "298\r\n     0\r\n";
  const MLeaderStyle taken = Read<MLeaderStyle>(text);
  CHECK(taken.Description == "Standard");
  CHECK(*taken.Leader == Leader{.Linetype = Handle{.Value = 0x14}});
  CHECK(taken.Text->Style == Handle{.Value = 0x11});
  CHECK(taken.Text->Bottom == Centered);
  CHECK(*taken.Block == Block{});
  CHECK_FALSE(taken.Annotative);
  CHECK(Written(taken) == text);
}

TEST_CASE("style: R2000's Annotative, its version kept in extended data")
{
  constexpr std::string_view text = "  0\r\nMLEADERSTYLE\r\n"
                                    "  5\r\n13B\r\n"
                                    "102\r\n{ACAD_REACTORS\r\n"
                                    "330\r\n12D\r\n"
                                    "102\r\n}\r\n"
                                    "330\r\n12D\r\n"
                                    "100\r\nAcDbMLeaderStyle\r\n"
                                    "170\r\n     2\r\n"
                                    "171\r\n     1\r\n"
                                    "172\r\n     0\r\n"
                                    " 90\r\n        2\r\n"
                                    " 40\r\n0.0\r\n"
                                    " 41\r\n0.0\r\n"
                                    "173\r\n     1\r\n"
                                    " 91\r\n-1056964608\r\n"
                                    "340\r\n14\r\n"
                                    " 92\r\n       -2\r\n"
                                    "290\r\n     1\r\n"
                                    " 42\r\n2.0\r\n"
                                    "291\r\n     1\r\n"
                                    " 43\r\n8.0\r\n"
                                    "  3\r\nStandard\r\n"
                                    " 44\r\n4.0\r\n"
                                    "300\r\n\r\n"
                                    "342\r\n11\r\n"
                                    "174\r\n     1\r\n"
                                    "178\r\n     1\r\n"
                                    "175\r\n     1\r\n"
                                    "176\r\n     0\r\n"
                                    " 93\r\n-1056964608\r\n"
                                    " 45\r\n4.0\r\n"
                                    "292\r\n     0\r\n"
                                    "297\r\n     0\r\n"
                                    " 46\r\n4.0\r\n"
                                    " 94\r\n-1056964608\r\n"
                                    " 47\r\n1.0\r\n"
                                    " 49\r\n1.0\r\n"
                                    "140\r\n1.0\r\n"
                                    "293\r\n     1\r\n"
                                    "141\r\n0.0\r\n"
                                    "294\r\n     1\r\n"
                                    "177\r\n     0\r\n"
                                    "142\r\n1.0\r\n"
                                    "295\r\n     0\r\n"
                                    "296\r\n     1\r\n"
                                    "143\r\n3.75\r\n"
                                    "1001\r\nACAD_MLEADERVER\r\n"
                                    "1070\r\n     2\r\n";
  const MLeaderStyle taken = Read<MLeaderStyle>(text, Version::R2000);
  CHECK(taken.Annotative);
  CHECK(taken.Base.Extended.size() == 1);
  CHECK(taken.Text->Direction == 0);
  CHECK(Written(taken, Version::R2000) == text);
}
} // namespace DxfIO::Objects::Mleader
