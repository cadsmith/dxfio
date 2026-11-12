#include "objects/shapes/dictionary.hpp"

#include "../kind.hpp"

#include <string_view>

namespace DxfIO::Objects
{
  TEST_CASE("dictionary: the root read from AutoCAD's record")
  {
    constexpr std::string_view text  = "  0\r\nDICTIONARY\r\n"
                                       "  5\r\nC\r\n"
                                       "330\r\n0\r\n"
                                       "100\r\nAcDbDictionary\r\n"
                                       "281\r\n     1\r\n"
                                       "  3\r\nACAD_CIP_PREVIOUS_PRODUCT_INFO\r\n"
                                       "350\r\n216\r\n"
                                       "  3\r\nACAD_COLOR\r\n"
                                       "350\r\n6B\r\n"
                                       "  3\r\nACAD_DETAILVIEWSTYLE\r\n"
                                       "350\r\n21B\r\n"
                                       "  3\r\nACAD_GROUP\r\n"
                                       "350\r\nD\r\n"
                                       "  3\r\nACAD_LAYOUT\r\n"
                                       "350\r\n1A\r\n"
                                       "  3\r\nACAD_MATERIAL\r\n"
                                       "350\r\n6A\r\n"
                                       "  3\r\nACAD_MLEADERSTYLE\r\n"
                                       "350\r\n12D\r\n"
                                       "  3\r\nACAD_MLINESTYLE\r\n"
                                       "350\r\n17\r\n"
                                       "  3\r\nACAD_PLOTSETTINGS\r\n"
                                       "350\r\n19\r\n"
                                       "  3\r\nACAD_PLOTSTYLENAME\r\n"
                                       "350\r\nE\r\n"
                                       "  3\r\nACAD_SCALELIST\r\n"
                                       "350\r\n10C\r\n"
                                       "  3\r\nACAD_SECTIONVIEWSTYLE\r\n"
                                       "350\r\n219\r\n"
                                       "  3\r\nACAD_TABLESTYLE\r\n"
                                       "350\r\n7E\r\n"
                                       "  3\r\nACAD_VISUALSTYLE\r\n"
                                       "350\r\nEF\r\n"
                                       "  3\r\nACDB_RECOMPOSE_DATA\r\n"
                                       "350\r\n2CD\r\n"
                                       "  3\r\nAcDbVariableDictionary\r\n"
                                       "350\r\n5E\r\n";
    const Dictionary           taken = Read<Dictionary>(text);
    CHECK_FALSE(taken.Hard);
    CHECK(taken.Cloning            == 1);
    REQUIRE(taken.Entries.size()   == 16);
    CHECK(taken.Entries.at(3).Name == "ACAD_GROUP");
    CHECK(taken.Entries.at(3).Soft == Handle{.Value = 0xD});
    CHECK(Written(taken)           == text);
  }

  TEST_CASE("dictionary: a hard owner names its objects on 360")
  {
    constexpr std::string_view text  = "  0\r\nDICTIONARY\r\n"
                                       "  5\r\n1FF\r\n"
                                       "330\r\n2\r\n"
                                       "100\r\nAcDbDictionary\r\n"
                                       "280\r\n     1\r\n"
                                       "281\r\n     1\r\n"
                                       "  3\r\nACAD_LAYERSTATES\r\n"
                                       "360\r\n200\r\n";
    const Dictionary           taken = Read<Dictionary>(text);
    CHECK(taken.Hard);
    REQUIRE(taken.Entries.size()     == 1);
    CHECK(taken.Entries.front().Hard == Handle{.Value = 0x200});
    CHECK_FALSE(taken.Entries.front().Soft);
    CHECK(Written(taken) == text);
  }

  TEST_CASE("dictionary: an entry pointing nowhere keeps its 350, as AutoCAD needs one")
  {
    constexpr std::string_view text  = "  0\r\nDICTIONARY\r\n"
                                       "  5\r\nC\r\n"
                                       "330\r\n0\r\n"
                                       "100\r\nAcDbDictionary\r\n"
                                       "281\r\n     1\r\n"
                                       "  3\r\nX\r\n"
                                       "350\r\n0\r\n";
    const Dictionary           taken = Read<Dictionary>(text);
    REQUIRE(taken.Entries.size() == 1);
    CHECK_FALSE(taken.Entries.front().Soft);
    CHECK_FALSE(taken.Entries.front().Hard);
    CHECK(Written(taken) == text);
  }

  TEST_CASE("dictionary: an empty one, and R13 without its flags")
  {
    constexpr std::string_view text  = "  0\r\nDICTIONARY\r\n"
                                       "  5\r\n13C\r\n"
                                       "330\r\n10\r\n"
                                       "100\r\nAcDbDictionary\r\n"
                                       "280\r\n     1\r\n"
                                       "281\r\n     1\r\n";
    const Dictionary           taken = Read<Dictionary>(text);
    CHECK(taken.Entries.empty());
    CHECK(Written(taken) == text);
    CHECK(Written(taken, Version::R14)
          == "  0\r\nDICTIONARY\r\n  5\r\n13C\r\n330\r\n10\r\n"
             "100\r\nAcDbDictionary\r\n");
  }
} // namespace DxfIO::Objects
