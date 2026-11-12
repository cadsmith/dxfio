#ifndef DXFIO_ENTITIES_SHAPES_SOLID_HPP
#define DXFIO_ENTITIES_SHAPES_SOLID_HPP

#include "filled.hpp"
#include "shapes.hpp"

#include "dxfio/entities/solid.hpp"

#include <string_view>

namespace DxfIO
{
  template <>
  inline constexpr auto Shapes<Entities::Solid> = Entities::Filled<Entities::Solid>;

  template <>
  inline constexpr std::string_view Type<Entities::Solid> = "SOLID";
} // namespace DxfIO

#endif /* DXFIO_ENTITIES_SHAPES_SOLID_HPP */
