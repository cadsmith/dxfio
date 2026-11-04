#ifndef DXFIO_OBJECTS_SHAPES_SCALE_HPP
#define DXFIO_OBJECTS_SHAPES_SCALE_HPP

#include "fields/field.hpp"
#include "fields/numbers.hpp"
#include "fields/subclass.hpp"
#include "fields/text.hpp"
#include "shapes.hpp"

#include "dxfio/objects/scale.hpp"

#include <string_view>
#include <tuple>

namespace DxfIO
{
template <>
inline constexpr auto Shapes<Objects::Scale> = std::tuple{
    Fields::Subclass{.Marker = "AcDbScale"},
    Fields::Field{.Code = 70, .Member = &Objects::Scale::Flags},
    Fields::Field{.Code = 300, .Member = &Objects::Scale::Name},
    Fields::Field{.Code = 140, .Member = &Objects::Scale::Paper},
    Fields::Field{.Code = 141, .Member = &Objects::Scale::Drawing},
    Fields::Field{.Code = 290, .Member = &Objects::Scale::Unit},
};

template <>
inline constexpr std::string_view Type<Objects::Scale> = "SCALE";
} // namespace DxfIO

#endif /* DXFIO_OBJECTS_SHAPES_SCALE_HPP */
