#include "objects/shapes/gathered.hpp"

#include "../kind.hpp"

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace DxfIO::Objects
{
namespace
{
auto Row(std::string_view type, std::string_view body) -> std::string
{
  return std::string{"  0\r\n"} + std::string{type} + "\r\n  5\r\n2A\r\n330\r\nC\r\n" + std::string{body};
}

template <class KindT>
auto Replaying(const std::string &text) -> KindT
{
  const KindT out = Read<KindT>(text);
  CHECK(out.Base.Extra.empty());
  CHECK(Written(out) == text);
  return out;
}
} // namespace

TEST_CASE("variable: a value as text, under its own class")
{
  const DictionaryVar one =
      Replaying<DictionaryVar>(Row("DICTIONARYVAR", "100\r\nDictionaryVariables\r\n280\r\n     0\r\n  1\r\n1:1\r\n"));
  CHECK(one.Schema == 0);
  CHECK(one.Value == "1:1");
}

TEST_CASE("xrecord: whatever it holds, in its order")
{
  const XRecord one = Replaying<XRecord>(Row("XRECORD", "100\r\nAcDbXrecord\r\n280\r\n     2\r\n  1\r\ntext\r\n"
                                                        " 70\r\n     7\r\n 40\r\n1.5\r\n 10\r\n1.0\r\n 20\r\n2.0\r\n"
                                                        " 30\r\n3.0\r\n330\r\nC\r\n300\r\ntail\r\n290\r\n     1\r\n"));
  CHECK(one.Cloning == 2);
  REQUIRE(one.Data.size() == 9);
  CHECK(one.Data.front() == DxfIO::Group{.Code = 1, .Value = std::string{"text"}});
  CHECK(one.Data.at(1) == DxfIO::Group{.Code = 70, .Value = std::int64_t{7}});
}

TEST_CASE("xrecord: a 280 of its data stays its data")
{
  const XRecord one = Replaying<XRecord>(Row("XRECORD", "100\r\nAcDbXrecord\r\n280\r\n     1\r\n"
                                                        "280\r\n     5\r\n"));
  CHECK(one.Cloning == 1);
  REQUIRE(one.Data.size() == 1);
  CHECK(one.Data.front().Code == 280);
}

TEST_CASE("xrecord: a 280 not first is its data, kept in its place")
{
  const XRecord one = Read<XRecord>(Row("XRECORD", "100\r\nAcDbXrecord\r\n  1\r\nx\r\n280\r\n     5\r\n  1\r\ny\r\n"));
  CHECK(one.Cloning == 1);
  REQUIRE(one.Data.size() == 3);
  CHECK(one.Data.at(1) == DxfIO::Group{.Code = 280, .Value = std::int64_t{5}});
  CHECK(Written(one) == Row("XRECORD", "100\r\nAcDbXrecord\r\n280\r\n     1\r\n"
                                       "  1\r\nx\r\n280\r\n     5\r\n  1\r\ny\r\n"));
}

TEST_CASE("xrecord: R14 has no cloning flag, its first 280 is data")
{
  const std::string text = Row("XRECORD", "100\r\nAcDbXrecord\r\n280\r\n     5\r\n  1\r\nx\r\n");
  const XRecord one = Read<XRecord>(text, Version::R14);
  CHECK(one.Cloning == 1);
  REQUIRE(one.Data.size() == 2);
  CHECK(one.Data.front() == DxfIO::Group{.Code = 280, .Value = std::int64_t{5}});
  CHECK(Written(one, Version::R14) == text);
}

TEST_CASE("xrecord: an empty one holds its flag alone, and nothing before R2000")
{
  const XRecord one = Replaying<XRecord>(Row("XRECORD", "100\r\nAcDbXrecord\r\n280\r\n     1\r\n"));
  CHECK(one.Data.empty());
  const std::string old = Row("XRECORD", "100\r\nAcDbXrecord\r\n");
  const XRecord bare = Read<XRecord>(old, Version::R14);
  CHECK(bare.Data.empty());
  CHECK(Written(bare, Version::R14) == old);
  const XRecord unflagged = Read<XRecord>(old);
  CHECK(unflagged.Data.empty());
  CHECK(Written(unflagged) == Row("XRECORD", "100\r\nAcDbXrecord\r\n280\r\n     1\r\n"));
}

TEST_CASE("xrecord: a first 280 holding no whole number is no flag")
{
  const std::vector<DxfIO::Group> worded{DxfIO::Group{.Code = 280, .Value = std::string{"x"}}};
  CHECK_FALSE(Flagged(worded, Version::R2018));
  const std::vector<DxfIO::Group> numbered{DxfIO::Group{.Code = 280, .Value = std::int64_t{3}}};
  CHECK(Flagged(numbered, Version::R2018) == 3);
}

TEST_CASE("xrecord: its data goes to every release")
{
  const XRecord one = Read<XRecord>(Row("XRECORD", "100\r\nAcDbXrecord\r\n280\r\n     1\r\n  1\r\nkept\r\n"));
  Fields::Sheet page{Version::R2000};
  page.To.From = Version::R2018;
  REQUIRE(Given(page.To, one));
  CHECK(page.Out.contains("  1\r\nkept\r\n"));
}

TEST_CASE("group: its members by handle, in their order")
{
  const Group one = Replaying<Group>(Row("GROUP", "100\r\nAcDbGroup\r\n300\r\ntwo lines\r\n 70\r\n     0\r\n"
                                                  " 71\r\n     1\r\n340\r\n2CC\r\n340\r\n2CD\r\n"));
  CHECK(one.Description == "two lines");
  CHECK(one.Selectable == 1);
  REQUIRE(one.Members.size() == 2);
  CHECK(one.Members.at(1) == Handle{0x2cd});
}

TEST_CASE("group: unnamed, with no description")
{
  const Group one = Replaying<Group>(Row("GROUP", "100\r\nAcDbGroup\r\n300\r\n\r\n 70\r\n     1\r\n"
                                                  " 71\r\n     1\r\n340\r\n2CE\r\n"));
  CHECK(one.Unnamed == 1);
  CHECK(one.Description.empty());
}

TEST_CASE("image: a definition, its reactor and the drawing's settings")
{
  const ImageDef file = Replaying<ImageDef>(Row("IMAGEDEF", "100\r\nAcDbRasterImageDef\r\n 90\r\n        0\r\n"
                                                            "  1\r\ndot.png\r\n 10\r\n4.0\r\n 20\r\n4.0\r\n"
                                                            " 11\r\n0.25\r\n 21\r\n0.25\r\n280\r\n     1\r\n"
                                                            "281\r\n     0\r\n"));
  CHECK(file.File == "dot.png");
  CHECK(file.Size == Vector{.X = 4, .Y = 4});
  const ImageDefReactor tie = Replaying<ImageDefReactor>(Row("IMAGEDEF_REACTOR", "100\r\nAcDbRasterImageDefReactor\r\n"
                                                                                 " 90\r\n        2\r\n330\r\n2D6\r\n"));
  CHECK(tie.Image == Handle{0x2d6});
  const RasterVariables all = Replaying<RasterVariables>(Row("RASTERVARIABLES", "100\r\nAcDbRasterVariables\r\n"
                                                                                " 90\r\n        0\r\n 70\r\n     1\r\n"
                                                                                " 71\r\n     1\r\n 72\r\n     1\r\n"));
  CHECK(all.Frame == 1);
}

TEST_CASE("image: a new reactor at version 2, as AutoCAD writes it")
{
  const ImageDefReactor fresh{};
  CHECK(fresh.Version == 2);
  CHECK(Written(fresh).contains(" 90\r\n        2\r\n"));
}
} // namespace DxfIO::Objects
