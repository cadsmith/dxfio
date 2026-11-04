#ifndef DXFIO_OBJECTS_SHAPES_LAYOUT_HPP
#define DXFIO_OBJECTS_SHAPES_LAYOUT_HPP

#include "fields/field.hpp"
#include "fields/flat.hpp"
#include "fields/handles.hpp"
#include "fields/numbers.hpp"
#include "fields/part.hpp"
#include "fields/points.hpp"
#include "fields/subclass.hpp"
#include "fields/text.hpp"
#include "plot.hpp"
#include "shapes.hpp"

#include "dxfio/handle.hpp"
#include "dxfio/objects/layout.hpp"
#include "dxfio/tables/axes.hpp"

#include <string_view>
#include <tuple>

namespace DxfIO
{
template <>
inline constexpr auto Shapes<Objects::Layout> = std::tuple{
    Fields::Part{
        .Member = &Objects::Layout::Plot,
        .Fields = Shapes<Objects::Plot>,
    },
    Fields::Subclass{.Marker = "AcDbLayout"},
    Fields::Field{.Code = 1, .Member = &Objects::Layout::Name},
    Fields::Field{.Code = 70, .Member = &Objects::Layout::Flags},
    Fields::Field{.Code = 71, .Member = &Objects::Layout::Order},
    Fields::Flat{.Code = 10, .Member = &Objects::Layout::Limmin},
    Fields::Flat{.Code = 11, .Member = &Objects::Layout::Limmax},
    Fields::Field{.Code = 12, .Member = &Objects::Layout::Insbase},
    Fields::Field{.Code = 14, .Member = &Objects::Layout::Extmin},
    Fields::Field{.Code = 15, .Member = &Objects::Layout::Extmax},
    Fields::Part{
        .Member = &Objects::Layout::Axes,
        .Fields =
            std::tuple{
                Fields::Field{.Code = 345, .Member = &Tables::Axes::Named, .Fallback = Handle{}},
                Fields::Field{.Code = 346, .Member = &Tables::Axes::Basis, .Fallback = Handle{}},
                Fields::Field{.Code = 146, .Member = &Tables::Axes::Elevation},
                Fields::Field{.Code = 13, .Member = &Tables::Axes::Origin},
                Fields::Field{.Code = 16, .Member = &Tables::Axes::X},
                Fields::Field{.Code = 17, .Member = &Tables::Axes::Y},
                Fields::Field{.Code = 76, .Member = &Tables::Axes::Ortho},
            },
    },
    Fields::Field{.Code = 330, .Member = &Objects::Layout::Block},
    Fields::Field{.Code = 331, .Member = &Objects::Layout::Viewport, .Fallback = Handle{}},
};

template <>
inline constexpr std::string_view Type<Objects::Layout> = "LAYOUT";
} // namespace DxfIO

#endif /* DXFIO_OBJECTS_SHAPES_LAYOUT_HPP */
