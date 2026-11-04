#include "dxfio/object.hpp"

#include <doctest/doctest.h>

#include <string_view>
#include <variant>

namespace DxfIO
{
TEST_CASE("object: an object tells its dxf name and its base")
{
  Object dictionary = Objects::Dictionary{};
  CHECK(TypeOf(dictionary) == "DICTIONARY");
  BaseOf(dictionary).Self = Handle{.Value = 0xc};
  CHECK(std::get<Objects::Dictionary>(dictionary).Base.Self == Handle{.Value = 0xc});
  const Object &fixed = dictionary;
  CHECK(BaseOf(fixed).Self == Handle{.Value = 0xc});
}

TEST_CASE("object: a dictionary with a default keeps its base in its dictionary")
{
  Object defaulted = Objects::DictionaryWithDefault{};
  CHECK(TypeOf(defaulted) == "ACDBDICTIONARYWDFLT");
  BaseOf(defaulted).Self = Handle{.Value = 0xd};
  CHECK(std::get<Objects::DictionaryWithDefault>(defaulted).Dictionary.Base.Self == Handle{.Value = 0xd});
  Object kept = Unknown{.Type = Symbol{"ACME_STORE"}, .Base = {}, .Groups = {}};
  CHECK(TypeOf(kept) == "ACME_STORE");
  CHECK_FALSE(BaseOf(kept).Self);
}

TEST_CASE("object: an unknown object's name outlives the record it was taken from")
{
  Object kept = Unknown{.Type = Symbol{"ACME_STORE"}, .Base = {}, .Groups = {}};
  const std::string_view name = TypeOf(kept);
  kept = Unknown{.Type = Symbol{"ACME_SHELF"}, .Base = {}, .Groups = {}};
  CHECK(name == "ACME_STORE");
}
} // namespace DxfIO
