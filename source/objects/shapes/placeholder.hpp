#ifndef DXFIO_OBJECTS_SHAPES_PLACEHOLDER_HPP
#define DXFIO_OBJECTS_SHAPES_PLACEHOLDER_HPP

#include "shapes.hpp"

#include "dxfio/objects/placeholder.hpp"

#include <string_view>
#include <tuple>

namespace DxfIO
{
  template <>
  inline constexpr auto Shapes<Objects::Placeholder> = std::tuple{};

  template <>
  inline constexpr std::string_view Type<Objects::Placeholder> = "ACDBPLACEHOLDER";
} // namespace DxfIO

#endif /* DXFIO_OBJECTS_SHAPES_PLACEHOLDER_HPP */
