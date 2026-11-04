#include "tables/shapes/ucs.hpp"

#include "../kind.hpp"

#include <string_view>

namespace DxfIO::Tables
{
TEST_CASE("ucs: read from AutoCAD's record and written back the same")
{
  constexpr std::string_view text = "  0\r\nUCS\r\n  5\r\n30\r\n330\r\n7\r\n100\r\nAcDbSymbolTableRecord\r\n"
                                    "100\r\nAcDbUCSTableRecord\r\n  2\r\nfront\r\n 70\r\n     0\r\n"
                                    " 10\r\n1.0\r\n 20\r\n2.0\r\n 30\r\n0.0\r\n"
                                    " 11\r\n1.0\r\n 21\r\n0.0\r\n 31\r\n0.0\r\n"
                                    " 12\r\n0.0\r\n 22\r\n0.0\r\n 32\r\n1.0\r\n"
                                    " 79\r\n     0\r\n146\r\n0.0\r\n";
  const Ucs taken = Read<Ucs>(text);
  CHECK(taken.Record.Name == "front");
  CHECK(taken.Origin == Vector{.X = 1, .Y = 2, .Z = 0});
  CHECK(taken.Y == Vector{.X = 0, .Y = 0, .Z = 1});
  CHECK_FALSE(taken.Basis);
  CHECK(Written(taken) == text);
}
} // namespace DxfIO::Tables
