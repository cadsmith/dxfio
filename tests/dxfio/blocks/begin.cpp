#include "blocks/shapes/begin.hpp"

#include "../kind.hpp"

#include <string_view>

namespace DxfIO::Blocks
{
TEST_CASE("begin: paper space's BLOCK read from AutoCAD's record")
{
  constexpr std::string_view text = "  0\r\nBLOCK\r\n"
                                    "  5\r\nD4\r\n"
                                    "330\r\nD2\r\n"
                                    "100\r\nAcDbEntity\r\n"
                                    " 67\r\n     1\r\n"
                                    "  8\r\n0\r\n"
                                    "100\r\nAcDbBlockBegin\r\n"
                                    "  2\r\n*Paper_Space\r\n"
                                    " 70\r\n     0\r\n"
                                    " 10\r\n0.0\r\n"
                                    " 20\r\n0.0\r\n"
                                    " 30\r\n0.0\r\n"
                                    "  3\r\n*Paper_Space\r\n"
                                    "  1\r\n\r\n";
  const Begin taken = Read<Begin>(text);
  CHECK(taken.Name == "*Paper_Space");
  CHECK(taken.Record.Paper);
  CHECK(taken.Record.Layer == "0");
  CHECK(taken.Record.Base.Owner == Handle{.Value = 0xd2});
  CHECK(taken.Record.Base.Extra.empty());
  CHECK(Written(taken) == text);
}

TEST_CASE("begin: R12's, no subclass, its name upper case")
{
  constexpr std::string_view text = "  0\r\nBLOCK\r\n"
                                    "  8\r\n0\r\n"
                                    "  2\r\n$MODEL_SPACE\r\n"
                                    " 70\r\n     0\r\n"
                                    " 10\r\n0.0\r\n"
                                    " 20\r\n0.0\r\n"
                                    " 30\r\n0.0\r\n"
                                    "  3\r\n$MODEL_SPACE\r\n"
                                    "  1\r\n\r\n";
  const Begin taken = Read<Begin>(text, Version::R12);
  CHECK(taken.Name == "$MODEL_SPACE");
  CHECK_FALSE(taken.Record.Paper);
  CHECK(Written(taken, Version::R12) == text);
}
} // namespace DxfIO::Blocks
