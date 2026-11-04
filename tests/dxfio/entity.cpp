#include "dxfio/entity.hpp"

#include <doctest/doctest.h>

#include <string_view>
#include <variant>

namespace DxfIO
{
TEST_CASE("entity: an entity tells its dxf name and its base")
{
  Entity line = Entities::Line{};
  CHECK(TypeOf(line) == "LINE");
  CHECK(TypeOf(Entity{Entities::LwPolyline{}}) == "LWPOLYLINE");
  BaseOf(line).Self = Handle{.Value = 0x2a};
  CHECK(std::get<Entities::Line>(line).Record.Base.Self == Handle{.Value = 0x2a});
  const Entity &fixed = line;
  CHECK(BaseOf(fixed).Self == Handle{.Value = 0x2a});
}

TEST_CASE("entity: an unknown entity tells the name it came in with")
{
  Entity kept = Unknown{.Type = Symbol{"ACME_GEAR"}, .Base = {}, .Groups = {}};
  CHECK(TypeOf(kept) == "ACME_GEAR");
  BaseOf(kept).Owner = Handle{.Value = 0x1f};
  CHECK(std::get<Unknown>(kept).Base.Owner == Handle{.Value = 0x1f});
}

TEST_CASE("entity: an unknown entity's name outlives the record it was taken from")
{
  Entity kept = Unknown{.Type = Symbol{"ACME_GEAR"}, .Base = {}, .Groups = {}};
  const std::string_view name = TypeOf(kept);
  kept = Unknown{.Type = Symbol{"ACME_BOLT"}, .Base = {}, .Groups = {}};
  CHECK(name == "ACME_GEAR");
}
} // namespace DxfIO
