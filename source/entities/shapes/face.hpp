#ifndef DXFIO_ENTITIES_SHAPES_FACE_HPP
#define DXFIO_ENTITIES_SHAPES_FACE_HPP

#include "fields/corners.hpp"
#include "fields/field.hpp"
#include "fields/numbers.hpp"
#include "fields/subclass.hpp"
#include "record.hpp"
#include "shapes.hpp"

#include "dxfio/entities/face.hpp"

#include <string_view>
#include <tuple>

namespace DxfIO
{
template <>
inline constexpr auto Shapes<Entities::Face3d> = std::tuple{
    Entities::Part<Entities::Face3d>,
    Fields::Subclass{.Marker = "AcDbFace"},
    Fields::Field{.Code = 10, .Member = &Entities::Face3d::Corners},
    Fields::Field{.Code = 70, .Member = &Entities::Face3d::Flags, .Fallback = 0},
};

template <>
inline constexpr std::string_view Type<Entities::Face3d> = "3DFACE";
} // namespace DxfIO

#endif /* DXFIO_ENTITIES_SHAPES_FACE_HPP */
