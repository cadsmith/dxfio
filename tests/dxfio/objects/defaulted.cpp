#include "objects/shapes/defaulted.hpp"

#include "../kind.hpp"

#include <string_view>

namespace DxfIO::Objects
{
TEST_CASE("defaulted: the plot style names read from AutoCAD's record")
{
  constexpr std::string_view text = "  0\r\nACDBDICTIONARYWDFLT\r\n"
                                    "  5\r\nE\r\n"
                                    "102\r\n{ACAD_REACTORS\r\n"
                                    "330\r\nC\r\n"
                                    "102\r\n}\r\n"
                                    "330\r\nC\r\n"
                                    "100\r\nAcDbDictionary\r\n"
                                    "281\r\n     1\r\n"
                                    "  3\r\nNormal\r\n"
                                    "350\r\nF\r\n"
                                    "100\r\nAcDbDictionaryWithDefault\r\n"
                                    "340\r\nF\r\n";
  const DictionaryWithDefault taken = Read<DictionaryWithDefault>(text);
  CHECK(taken.Dictionary.Base.Self == Handle{.Value = 0xE});
  CHECK(taken.Dictionary.Base.Reactors == std::vector{Handle{.Value = 0xC}});
  REQUIRE(taken.Dictionary.Entries.size() == 1);
  CHECK(taken.Dictionary.Entries.front().Name == "Normal");
  CHECK(taken.Fallback == Handle{.Value = 0xF});
  CHECK(Written(taken) == text);
}

TEST_CASE("defaulted: an entry pointing nowhere keeps its 350")
{
  constexpr std::string_view text = "  0\r\nACDBDICTIONARYWDFLT\r\n"
                                    "  5\r\nE\r\n"
                                    "330\r\nC\r\n"
                                    "100\r\nAcDbDictionary\r\n"
                                    "281\r\n     1\r\n"
                                    "  3\r\nNormal\r\n"
                                    "350\r\n0\r\n"
                                    "100\r\nAcDbDictionaryWithDefault\r\n"
                                    "340\r\nF\r\n";
  const DictionaryWithDefault taken = Read<DictionaryWithDefault>(text);
  REQUIRE(taken.Dictionary.Entries.size() == 1);
  CHECK_FALSE(taken.Dictionary.Entries.front().Soft);
  CHECK(Written(taken) == text);
}
} // namespace DxfIO::Objects
