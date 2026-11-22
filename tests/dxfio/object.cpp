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

  TEST_CASE("object: a dictionary with a default goes by the type of one")
  {
    Object defaulted = Objects::Dictionary{.Default = Handle{}};
    CHECK(TypeOf(defaulted) == "ACDBDICTIONARYWDFLT");
    BaseOf(defaulted).Self = Handle{.Value = 0xd};
    CHECK(std::get<Objects::Dictionary>(defaulted).Base.Self == Handle{.Value = 0xd});
    Object kept = Unknown{.Type = Symbol{"ACME_STORE"}, .Base = {}, .Tags = {}};
    CHECK(TypeOf(kept) == "ACME_STORE");
    CHECK_FALSE(BaseOf(kept).Self);
  }

  TEST_CASE("object: an unknown object's name outlives the record it was taken from")
  {
    Object                 kept = Unknown{.Type = Symbol{"ACME_STORE"}, .Base = {}, .Tags = {}};
    const std::string_view name = TypeOf(kept);
    kept                        = Unknown{.Type = Symbol{"ACME_SHELF"}, .Base = {}, .Tags = {}};
    CHECK(name == "ACME_STORE");
  }
} // namespace DxfIO
