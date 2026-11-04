#include "dxfio/object.hpp"

#include "based.hpp"
#include "objects/io.hpp"
#include "objects/shapes/defaulted.hpp"

#include <variant>

namespace DxfIO
{
auto TypeOf(const Object &one) -> std::string_view
{
  return Objects::Type(one);
}

auto BaseOf(const Object &one) -> const Base &
{
  return Objects::Based(one);
}

auto BaseOf(Object &one) -> Base &
{
  return std::visit(
      [](auto &kind) -> Base &
      {
        using DxfIO::Based;
        return Based(kind);
      },
      one);
}
} // namespace DxfIO
