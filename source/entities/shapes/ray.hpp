#ifndef DXFIO_ENTITIES_SHAPES_RAY_HPP
#define DXFIO_ENTITIES_SHAPES_RAY_HPP

#include "fields/field.hpp"
#include "fields/points.hpp"
#include "fields/subclass.hpp"
#include "record.hpp"
#include "shapes.hpp"

#include "dxfio/entities/ray.hpp"

#include <string_view>
#include <tuple>

namespace DxfIO
{
template <>
inline constexpr auto Shapes<Entities::Ray> = std::tuple{
    Entities::Part<Entities::Ray>,
    Fields::Subclass{.Marker = "AcDbRay"},
    Fields::Field{.Code = 10, .Member = &Entities::Ray::Start},
    Fields::Field{.Code = 11, .Member = &Entities::Ray::Direction},
};

template <>
inline constexpr std::string_view Type<Entities::Ray> = "RAY";
} // namespace DxfIO

#endif /* DXFIO_ENTITIES_SHAPES_RAY_HPP */
