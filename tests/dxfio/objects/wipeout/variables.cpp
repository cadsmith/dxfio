#include "objects/shapes/wipeout/variables.hpp"

#include "../../kind.hpp"

#include <string_view>

namespace DxfIO::Objects
{
  TEST_CASE("wipeout variables: its frame, read from AutoCAD's record")
  {
    constexpr std::string_view text  = "  0\r\nWIPEOUTVARIABLES\r\n"
                                       "  5\r\n2D5\r\n"
                                       "102\r\n{ACAD_REACTORS\r\n"
                                       "330\r\nC\r\n"
                                       "102\r\n}\r\n"
                                       "330\r\nC\r\n"
                                       "100\r\nAcDbWipeoutVariables\r\n"
                                       " 70\r\n     1\r\n";
    const WipeoutVariables     taken = Read<WipeoutVariables>(text);
    CHECK(taken.Frame == 1);
    CHECK(taken.Base.Reactors.size() == 1);
    CHECK(Written(taken)             == text);
  }
} // namespace DxfIO::Objects
