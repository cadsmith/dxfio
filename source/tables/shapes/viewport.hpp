#ifndef DXFIO_TABLES_SHAPES_VIEWPORT_HPP
#define DXFIO_TABLES_SHAPES_VIEWPORT_HPP

#include "axes.hpp"
#include "fields/flat.hpp"
#include "fields/optional.hpp"
#include "fields/part.hpp"
#include "fields/since.hpp"
#include "record.hpp"

#include "dxfio/tables/vport.hpp"

#include <string_view>
#include <tuple>

namespace DxfIO
{
namespace Tables
{
inline constexpr auto Frame = std::tuple{
    Fields::Flat{.Code = 10, .Member = &Vport::Lower},      Fields::Flat{.Code = 11, .Member = &Vport::Upper},
    Fields::Flat{.Code = 12, .Member = &Vport::Center},     Fields::Flat{.Code = 13, .Member = &Vport::Base},
    Fields::Flat{.Code = 14, .Member = &Vport::Snap},       Fields::Flat{.Code = 15, .Member = &Vport::Grid},
    Fields::Field{.Code = 16, .Member = &Vport::Direction}, Fields::Field{.Code = 17, .Member = &Vport::Target},
    Fields::Field{.Code = 40, .Member = &Vport::Height},    Fields::Field{.Code = 41, .Member = &Vport::Aspect},
    Fields::Field{.Code = 42, .Member = &Vport::Focal},     Fields::Field{.Code = 43, .Member = &Vport::Front},
    Fields::Field{.Code = 44, .Member = &Vport::Back},      Fields::Field{.Code = 50, .Member = &Vport::Rotation},
    Fields::Field{.Code = 51, .Member = &Vport::Twist},
};

inline constexpr auto Modes = std::tuple{
    Fields::Field{.Code = 71, .Member = &Vport::Mode},      Fields::Field{.Code = 72, .Member = &Vport::Zoom},
    Fields::Field{.Code = 73, .Member = &Vport::Fast},      Fields::Field{.Code = 74, .Member = &Vport::Icon},
    Fields::Field{.Code = 75, .Member = &Vport::Snapped},   Fields::Field{.Code = 76, .Member = &Vport::Gridded},
    Fields::Field{.Code = 77, .Member = &Vport::Isometric}, Fields::Field{.Code = 78, .Member = &Vport::Isoplane},
};

inline constexpr auto Oriented = std::tuple{
    Fields::Field{.Code = 281, .Member = &Vport::Render},
    Fields::Field{.Code = 65, .Member = &Vport::Ucsvp},
    Fields::Part{.Member = &Vport::Axes, .Fields = Shapes<Axes>},
};

inline constexpr auto Lights = std::tuple{
    Fields::Field{.Code = 348, .Member = &Vport::Style},
    Fields::Field{.Code = 60, .Member = &Vport::Display},
    Fields::Field{.Code = 61, .Member = &Vport::Major},
    Fields::Field{.Code = 332, .Member = &Vport::Background, .Fallback = Handle{}},
    Fields::Field{.Code = 333, .Member = &Vport::Shadow, .Fallback = Handle{}},
    Fields::Field{.Code = 292, .Member = &Vport::Lit},
    Fields::Field{.Code = 282, .Member = &Vport::Lighting},
    Fields::Field{.Code = 141, .Member = &Vport::Brightness},
    Fields::Field{.Code = 142, .Member = &Vport::Contrast},
    Fields::Field{.Code = 63, .Member = &Vport::Ambient},
    Fields::Field{.Code = 421, .Member = &Vport::Rgb},
};
} // namespace Tables

template <>
inline constexpr auto Shapes<Tables::Vport> = std::tuple_cat(
    std::tuple{
        Tables::Opening,
        Fields::Subclass{.Marker = "AcDbViewportTableRecord"},
        Fields::Part{
            .Member = &Tables::Vport::Record,
            .Fields = Shapes<Tables::Record>,
        },
    },
    Tables::Frame, Tables::Modes,
    std::tuple{
        Fields::Since{.Release = Version::R2000, .Fields = Tables::Oriented},
        Fields::Since{.Release = Version::R2007, .Fields = Tables::Lights},
    });

template <>
inline constexpr std::string_view Type<Tables::Vport> = "VPORT";
} // namespace DxfIO

#endif /* DXFIO_TABLES_SHAPES_VIEWPORT_HPP */
