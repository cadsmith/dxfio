#include "tables/shapes/block.hpp"

#include "../entities/drawn.hpp"
#include "../kind.hpp"

#include <string>
#include <string_view>
#include <vector>

namespace DxfIO::Tables
{
TEST_CASE("block: a block's record read from AutoCAD's record")
{
  constexpr std::string_view text = "  0\r\nBLOCK_RECORD\r\n"
                                    "  5\r\n2D9\r\n"
                                    "330\r\n1\r\n"
                                    "100\r\nAcDbSymbolTableRecord\r\n"
                                    "100\r\nAcDbBlockTableRecord\r\n"
                                    "  2\r\nmark\r\n"
                                    "340\r\n0\r\n"
                                    " 70\r\n     4\r\n"
                                    "280\r\n     1\r\n"
                                    "281\r\n     0\r\n";
  const BlockRecord taken = Read<BlockRecord>(text);
  CHECK(taken.Record.Name == "mark");
  CHECK(taken.Layout == Handle{});
  CHECK(taken.Units == 4);
  CHECK(taken.Explodable);
  CHECK(Written(taken) == text);
}

TEST_CASE("block: model space owns a layout and a dictionary")
{
  constexpr std::string_view text = "  0\r\nBLOCK_RECORD\r\n"
                                    "  5\r\n1F\r\n"
                                    "102\r\n{ACAD_XDICTIONARY\r\n"
                                    "360\r\n1CE\r\n"
                                    "102\r\n}\r\n"
                                    "330\r\n1\r\n"
                                    "100\r\nAcDbSymbolTableRecord\r\n"
                                    "100\r\nAcDbBlockTableRecord\r\n"
                                    "  2\r\n*Model_Space\r\n"
                                    "340\r\n22\r\n"
                                    " 70\r\n     0\r\n"
                                    "280\r\n     1\r\n"
                                    "281\r\n     0\r\n";
  const BlockRecord taken = Read<BlockRecord>(text);
  CHECK(taken.Layout == Handle{.Value = 0x22});
  CHECK(taken.Record.Base.Dictionary == Handle{.Value = 0x1CE});
  CHECK(Written(taken) == text);
}

TEST_CASE("block: R2000 keeps the units in extended data")
{
  constexpr std::string_view text = "  0\r\nBLOCK_RECORD\r\n"
                                    "  5\r\n2D9\r\n"
                                    "330\r\n1\r\n"
                                    "100\r\nAcDbSymbolTableRecord\r\n"
                                    "100\r\nAcDbBlockTableRecord\r\n"
                                    "  2\r\nmark\r\n"
                                    "340\r\n0\r\n"
                                    "1001\r\nACAD\r\n"
                                    "1000\r\nDesignCenter Data\r\n"
                                    "1002\r\n{\r\n"
                                    "1070\r\n     1\r\n"
                                    "1070\r\n     4\r\n"
                                    "1002\r\n}\r\n";
  const BlockRecord taken = Read<BlockRecord>(text, Version::R2000);
  CHECK(taken.Units == 0);
  REQUIRE(taken.Record.Base.Extended.size() == 1);
  CHECK(taken.Record.Base.Extended.at(0).Application == "ACAD");
  CHECK(Written(taken, Version::R2000) == text);
}

TEST_CASE("block: its inserts in their brace after 340, as AutoCAD writes them")
{
  const std::string text = Entities::Drawn("2D0", Version::R2018, "inserts");
  const BlockRecord taken = Read<BlockRecord>(text);
  CHECK(taken.Record.Name == "plain");
  REQUIRE(taken.Inserts.size() == 12);
  CHECK(taken.Inserts.front() == Handle{.Value = 0x301});
  CHECK(taken.Inserts.back() == Handle{.Value = 0x2fb});
  CHECK(taken.Record.Base.Extra.empty());
  CHECK(Written(taken) == text);
  const std::string old = Entities::Drawn("2D0", Version::R2000, "inserts");
  const BlockRecord early = Read<BlockRecord>(old, Version::R2000);
  CHECK(early.Inserts == taken.Inserts);
  CHECK(Written(early, Version::R2000) == old);
  CHECK_FALSE(Written(early, Version::R14).contains("BLKREFS"));
}

TEST_CASE("block: a brace not of inserts, or not whole, stays as it came")
{
  const std::string head = "  0\r\nBLOCK_RECORD\r\n  5\r\n2D9\r\n330\r\n1\r\n100\r\nAcDbSymbolTableRecord\r\n"
                           "100\r\nAcDbBlockTableRecord\r\n  2\r\nmark\r\n340\r\n0\r\n";
  const std::string tail = " 70\r\n     0\r\n280\r\n     1\r\n281\r\n     0\r\n";
  const std::vector<std::string> braces{
      "102\r\n{OTHER\r\n331\r\n2A\r\n102\r\n}\r\n",
      "102\r\n{BLKREFS\r\n331\r\n2A\r\n",
      "102\r\n{BLKREFS\r\n331\r\n2A\r\n  1\r\nodd\r\n102\r\n}\r\n",
      "102\r\n{BLKREFS\r\n331\r\n2A\r\n102\r\n{\r\n",
  };
  for (const std::string &brace : braces)
  {
    CAPTURE(brace);
    const BlockRecord taken = Read<BlockRecord>(head + brace + tail);
    CHECK(taken.Inserts.empty());
    CHECK(Written(taken) == head + tail + brace);
  }
  const std::string open = "102\r\n{BLKREFS\r\n331\r\n2A\r\n";
  const BlockRecord last = Read<BlockRecord>(head + open, Version::R2000);
  CHECK(last.Inserts.empty());
  CHECK(Written(last, Version::R2000) == head + open);
}

TEST_CASE("block: R13 and R14 have no layouts, so no 340")
{
  constexpr std::string_view text = "  0\r\nBLOCK_RECORD\r\n"
                                    "  5\r\n2D9\r\n"
                                    "330\r\n1\r\n"
                                    "100\r\nAcDbSymbolTableRecord\r\n"
                                    "100\r\nAcDbBlockTableRecord\r\n"
                                    "  2\r\nmark\r\n";
  const BlockRecord taken = Read<BlockRecord>(text, Version::R14);
  CHECK(Written(taken, Version::R14) == text);
  BlockRecord model = taken;
  model.Layout = Handle{.Value = 0x22};
  CHECK(Written(model, Version::R13) == text);
}
} // namespace DxfIO::Tables
