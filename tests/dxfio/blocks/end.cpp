#include "blocks/shapes/end.hpp"

#include "../kind.hpp"

#include <string_view>

namespace DxfIO::Blocks
{
  TEST_CASE("end: model space's ENDBLK read from AutoCAD's record")
  {
    constexpr std::string_view text  = "  0\r\nENDBLK\r\n"
                                       "  5\r\n21\r\n"
                                       "330\r\n1F\r\n"
                                       "100\r\nAcDbEntity\r\n"
                                       "  8\r\n0\r\n"
                                       "100\r\nAcDbBlockEnd\r\n";
    const End                  taken = Read<End>(text);
    CHECK(taken.Record.Base.Self == Handle{.Value = 0x21});
    CHECK_FALSE(taken.Record.Paper);
    CHECK(Written(taken) == text);
  }
} // namespace DxfIO::Blocks
