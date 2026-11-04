#ifndef DXFIO_ENTITIES_SHAPES_HELIX_HPP
#define DXFIO_ENTITIES_SHAPES_HELIX_HPP

#include "fields/field.hpp"
#include "fields/numbers.hpp"
#include "fields/part.hpp"
#include "fields/picture.hpp"
#include "fields/points.hpp"
#include "fields/subclass.hpp"
#include "record.hpp"
#include "shapes.hpp"
#include "spline.hpp"

#include "dxfio/entities/helix.hpp"

#include <string_view>
#include <tuple>

namespace DxfIO
{
template <>
inline constexpr auto Shapes<Entities::Coil> = std::tuple{
    Fields::Subclass{.Marker = "AcDbHelix"},
    Fields::Field{.Code = 90, .Member = &Entities::Coil::Major},
    Fields::Field{.Code = 91, .Member = &Entities::Coil::Minor},
    Fields::Field{.Code = 10, .Member = &Entities::Coil::Base},
    Fields::Field{.Code = 11, .Member = &Entities::Coil::Start},
    Fields::Field{.Code = 12, .Member = &Entities::Coil::Axis},
    Fields::Field{.Code = 40, .Member = &Entities::Coil::Radius},
    Fields::Field{.Code = 41, .Member = &Entities::Coil::Turns},
    Fields::Field{.Code = 42, .Member = &Entities::Coil::Rise},
    Fields::Field{.Code = 290, .Member = &Entities::Coil::Right},
    Fields::Field{.Code = 280, .Member = &Entities::Coil::Constraint},
};

template <>
inline constexpr auto Shapes<Entities::Helix> = std::tuple{
    Entities::Part<Entities::Helix>,
    Fields::Picture{.Member = &Entities::Helix::Picture},
    Fields::Part{
        .Member = &Entities::Helix::Curve,
        .Fields = Shapes<Entities::Curve>,
    },
    Fields::Part{
        .Member = &Entities::Helix::Coil,
        .Fields = Shapes<Entities::Coil>,
    },
};

template <>
inline constexpr std::string_view Type<Entities::Helix> = "HELIX";
} // namespace DxfIO

#endif /* DXFIO_ENTITIES_SHAPES_HELIX_HPP */
