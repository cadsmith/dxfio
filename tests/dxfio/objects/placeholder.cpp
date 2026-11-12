#include "objects/shapes/placeholder.hpp"

#include "../kind.hpp"

#include <string_view>

namespace DxfIO::Objects
{
  TEST_CASE("placeholder: only its head, read from AutoCAD's record")
  {
    constexpr std::string_view text  = "  0\r\nACDBPLACEHOLDER\r\n"
                                       "  5\r\nF\r\n"
                                       "102\r\n{ACAD_REACTORS\r\n"
                                       "330\r\nE\r\n"
                                       "102\r\n}\r\n"
                                       "330\r\nE\r\n";
    const Placeholder          taken = Read<Placeholder>(text);
    CHECK(taken.Base.Self  == Handle{.Value = 0xF});
    CHECK(taken.Base.Owner == Handle{.Value = 0xE});
    CHECK(Written(taken)   == text);
  }
} // namespace DxfIO::Objects
