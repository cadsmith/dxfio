#include "tables/shapes/appid.hpp"

#include "../kind.hpp"

#include <string_view>

namespace DxfIO::Tables
{
  TEST_CASE("appid: read from AutoCAD's record and written back the same")
  {
    constexpr std::string_view text =
        "  0\r\nAPPID\r\n  "
        "5\r\n12\r\n330\r\n9\r\n100\r\nAcDbSymbolTableRecord\r\n"
        "100\r\nAcDbRegAppTableRecord\r\n  2\r\nACAD\r\n 70\r\n     0\r\n";
    const Appid taken = Read<Appid>(text);
    CHECK(taken.Record.Name      == "ACAD");
    CHECK(taken.Record.Base.Self == Handle{0x12});
    CHECK(Written(taken)         == text);
  }

  TEST_CASE("appid: what no field took goes back to its own release alone")
  {
    const Appid taken =
        Read<Appid>("  0\nAPPID\n100\nAcDbSymbolTableRecord\n100\nAcDbRegAppTableRecord\n"
                    "  2\nKADXY\n 70\n0\n 71\n1\n");
    CHECK(taken.Record.Base.Extra.size() == 1);
    CHECK(Written(taken).find(" 71") != std::string::npos);
    Fields::Sheet other;
    other.To.From = Version::R2004;
    REQUIRE(Given(other.To, taken));
    CHECK(other.Out.find(" 71") == std::string::npos);
  }
} // namespace DxfIO::Tables
