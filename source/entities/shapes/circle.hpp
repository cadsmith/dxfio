#ifndef DXFIO_ENTITIES_SHAPES_CIRCLE_HPP
#define DXFIO_ENTITIES_SHAPES_CIRCLE_HPP

#include "round.hpp"
#include "shapes.hpp"

#include "dxfio/entities/circle.hpp"

#include <string_view>

namespace DxfIO
{
template <>
inline constexpr auto Shapes<Entities::Circle> = Entities::Round<Entities::Circle>;

template <>
inline constexpr std::string_view Type<Entities::Circle> = "CIRCLE";
} // namespace DxfIO

#endif /* DXFIO_ENTITIES_SHAPES_CIRCLE_HPP */
