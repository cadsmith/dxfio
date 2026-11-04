#include "tables/shapes/style.hpp"

#include "../kind.hpp"

#include <string_view>

namespace DxfIO::Tables
{
TEST_CASE("style: read from AutoCAD's record and written back the same")
{
  constexpr std::string_view text = "  0\r\nSTYLE\r\n  "
                                    "5\r\n11\r\n330\r\n3\r\n100\r\nAcDbSymbolTableRecord\r\n"
                                    "100\r\nAcDbTextStyleTableRecord\r\n  2\r\nStandard\r\n 70\r\n     0\r\n"
                                    " 40\r\n0.0\r\n 41\r\n1.0\r\n 50\r\n0.0\r\n 71\r\n     0\r\n"
                                    " 42\r\n2.5\r\n  3\r\narial.ttf\r\n  4\r\n\r\n";
  const Style taken = Read<Style>(text);
  CHECK(taken.Record.Name == "Standard");
  CHECK(taken.Font == "arial.ttf");
  CHECK(taken.Bigfont.empty());
  CHECK(taken.Previous == 2.5);
  CHECK(Written(taken) == text);
}
} // namespace DxfIO::Tables
