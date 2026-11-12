#include "dxfio/entity.hpp"

#include "based.hpp"
#include "entities/io.hpp"

#include <variant>

namespace DxfIO
{
  auto TypeOf(const Entity &one) -> std::string_view
  {
    return Entities::Named(one);
  }

  auto BaseOf(const Entity &one) -> const Base &
  {
    return Entities::Based(one);
  }

  auto BaseOf(Entity &one) -> Base &
  {
    return std::visit([](auto &kind) -> Base & { return DxfIO::Based(kind); }, one);
  }
} // namespace DxfIO
