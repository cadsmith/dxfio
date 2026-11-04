#include "../fields/fixture.hpp"
#include "file/sections/records.hpp"
#include "objects/io.hpp"
#include "tags/tags.hpp"

#include <doctest/doctest.h>

#include <string_view>
#include <variant>
#include <vector>

namespace DxfIO::Objects
{
namespace
{
constexpr std::string_view Text = "  0\r\nDICTIONARY\r\n  5\r\nC\r\n330\r\n0\r\n"
                                  "100\r\nAcDbDictionary\r\n281\r\n     1\r\n"
                                  "  3\r\nACAD_GROUP\r\n350\r\nD\r\n"
                                  "  0\r\nACDBNOTHING\r\n  5\r\nD\r\n330\r\nC\r\n 90\r\n        7\r\n";
} // namespace

TEST_CASE("objects: each of the kind its type names, or kept unknown")
{
  const auto all = Tags::All(Text);
  REQUIRE(all);
  const auto records = File::Sections::Records(*all);
  REQUIRE(records);
  std::vector<Issue> issues;
  const std::vector<Object> read = Objects::Read(*records, Fields::Reading{.Issues = &issues});
  CHECK(issues.empty());
  REQUIRE(read.size() == 2);
  REQUIRE(std::holds_alternative<Dictionary>(read.front()));
  CHECK(std::get<Dictionary>(read.front()).Entries.size() == 1);
  REQUIRE(std::holds_alternative<Unknown>(read.back()));
  CHECK(std::get<Unknown>(read.back()).Type == "ACDBNOTHING");
  Fields::Sheet page;
  REQUIRE(Write(page.To, read));
  CHECK(page.Out == Text);
}

TEST_CASE("objects: the base of each kind, a defaulted dictionary's too")
{
  const Handle self{.Value = 0x2a};
  Dictionary plain{};
  plain.Base.Self = self;
  CHECK(Based(Object{plain}).Self == self);
  DictionaryWithDefault fallback{};
  fallback.Dictionary = plain;
  CHECK(Based(Object{fallback}).Self == self);
  Unknown kept{};
  kept.Base.Self = self;
  CHECK(Based(Object{kept}).Self == self);
}

TEST_CASE("objects: the type an object is written under")
{
  CHECK(Type(Object{Scale{}}) == "SCALE");
  CHECK(Type(Object{MLineStyle{}}) == "MLINESTYLE");
  CHECK(Type(Object{Unknown{.Type = Symbol{"ACDBNOTHING"}, .Base = {}, .Groups = {}}}) == "ACDBNOTHING");
}

TEST_CASE("objects: a material's maps on the heap, copied with it")
{
  CHECK(sizeof(Object) < sizeof(Maps) / 2);
  CHECK(sizeof(Layout) < sizeof(Plot) * 2);
  Material one{};
  one.Maps->Bump.File = "bump.png";
  const Object copy{one};
  one.Maps->Bump.File.clear();
  CHECK(std::get<Material>(copy).Maps->Bump.File == "bump.png");
}
} // namespace DxfIO::Objects
