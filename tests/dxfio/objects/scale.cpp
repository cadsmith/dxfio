#include "objects/shapes/scale.hpp"

#include "../kind.hpp"

#include <string_view>

namespace DxfIO::Objects
{
  TEST_CASE("scale: the one to one scale read from AutoCAD's record")
  {
    constexpr std::string_view text  = "  0\r\nSCALE\r\n"
                                       "  5\r\n10D\r\n"
                                       "102\r\n{ACAD_REACTORS\r\n"
                                       "330\r\n10C\r\n"
                                       "102\r\n}\r\n"
                                       "330\r\n10C\r\n"
                                       "100\r\nAcDbScale\r\n"
                                       " 70\r\n     0\r\n"
                                       "300\r\n1:1\r\n"
                                       "140\r\n1.0\r\n"
                                       "141\r\n1.0\r\n"
                                       "290\r\n     1\r\n";
    const Scale                taken = Read<Scale>(text);
    CHECK(taken.Name == "1:1");
    CHECK(taken.Unit);
    CHECK(Written(taken) == text);
  }

  TEST_CASE("scale: a scale of two drawing units to one")
  {
    constexpr std::string_view text  = "  0\r\nSCALE\r\n"
                                       "  5\r\n1BE\r\n"
                                       "102\r\n{ACAD_REACTORS\r\n"
                                       "330\r\n10C\r\n"
                                       "102\r\n}\r\n"
                                       "330\r\n10C\r\n"
                                       "100\r\nAcDbScale\r\n"
                                       " 70\r\n     0\r\n"
                                       "300\r\n1:2\r\n"
                                       "140\r\n1.0\r\n"
                                       "141\r\n2.0\r\n"
                                       "290\r\n     0\r\n";
    const Scale                taken = Read<Scale>(text);
    CHECK(taken.Name    == "1:2");
    CHECK(taken.Drawing == 2.0);
    CHECK_FALSE(taken.Unit);
    CHECK(Written(taken) == text);
  }
} // namespace DxfIO::Objects
