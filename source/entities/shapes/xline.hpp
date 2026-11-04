#ifndef DXFIO_ENTITIES_SHAPES_XLINE_HPP
#define DXFIO_ENTITIES_SHAPES_XLINE_HPP

#include "fields/field.hpp"
#include "fields/points.hpp"
#include "fields/subclass.hpp"
#include "record.hpp"
#include "shapes.hpp"

#include "dxfio/entities/xline.hpp"

#include <string_view>
#include <tuple>

namespace DxfIO
{
template <>
inline constexpr auto Shapes<Entities::XLine> = std::tuple{
    Entities::Part<Entities::XLine>,
    Fields::Subclass{.Marker = "AcDbXline"},
    Fields::Field{.Code = 10, .Member = &Entities::XLine::Start},
    Fields::Field{.Code = 11, .Member = &Entities::XLine::Direction},
};

template <>
inline constexpr std::string_view Type<Entities::XLine> = "XLINE";
} // namespace DxfIO

#endif /* DXFIO_ENTITIES_SHAPES_XLINE_HPP */
