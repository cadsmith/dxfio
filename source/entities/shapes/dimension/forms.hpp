#ifndef DXFIO_ENTITIES_SHAPES_DIMENSION_FORMS_HPP
#define DXFIO_ENTITIES_SHAPES_DIMENSION_FORMS_HPP

#include "fields/field.hpp"
#include "fields/numbers.hpp"
#include "fields/points.hpp"
#include "fields/skip.hpp"
#include "fields/subclass.hpp"
#include "shapes.hpp"

#include "dxfio/entities/dimension/dimension.hpp"

#include <string_view>
#include <tuple>

namespace DxfIO
{
namespace Entities::Forms
{
inline constexpr std::string_view Along = "AcDbAlignedDimension";
} // namespace Entities::Forms

template <>
inline constexpr auto Shapes<Entities::Forms::Linear> = std::tuple{
    Fields::Subclass{.Marker = Entities::Forms::Along},
    Fields::Field{.Code = 13, .Member = &Entities::Forms::Linear::Start},
    Fields::Field{.Code = 14, .Member = &Entities::Forms::Linear::End},
    Fields::Field{
        .Code = 50,
        .Member = &Entities::Forms::Linear::Angle,
        .Fallback = 0.0,
    },
    Fields::Skip{.Code = 40},
    Fields::Subclass{.Marker = "AcDbRotatedDimension"},
};

template <>
inline constexpr auto Shapes<Entities::Forms::Aligned> = std::tuple{
    Fields::Subclass{.Marker = Entities::Forms::Along},
    Fields::Field{.Code = 13, .Member = &Entities::Forms::Aligned::Start},
    Fields::Field{.Code = 14, .Member = &Entities::Forms::Aligned::End},
    Fields::Field{
        .Code = 50,
        .Member = &Entities::Forms::Aligned::Angle,
        .Fallback = 0.0,
    },
};

template <>
inline constexpr auto Shapes<Entities::Forms::Angular> = std::tuple{
    Fields::Subclass{.Marker = "AcDb2LineAngularDimension"},
    Fields::Field{.Code = 13, .Member = &Entities::Forms::Angular::Start},
    Fields::Field{.Code = 14, .Member = &Entities::Forms::Angular::End},
    Fields::Field{.Code = 15, .Member = &Entities::Forms::Angular::From},
    Fields::Field{.Code = 16, .Member = &Entities::Forms::Angular::To},
};

template <>
inline constexpr auto Shapes<Entities::Forms::Diameter> = std::tuple{
    Fields::Subclass{.Marker = "AcDbDiametricDimension"},
    Fields::Field{.Code = 15, .Member = &Entities::Forms::Diameter::Chord},
    Fields::Field{.Code = 40, .Member = &Entities::Forms::Diameter::Leader},
};

template <>
inline constexpr auto Shapes<Entities::Forms::Radial> = std::tuple{
    Fields::Subclass{.Marker = "AcDbRadialDimension"},
    Fields::Field{.Code = 15, .Member = &Entities::Forms::Radial::Chord},
    Fields::Field{.Code = 40, .Member = &Entities::Forms::Radial::Leader},
};

template <>
inline constexpr auto Shapes<Entities::Forms::Points> = std::tuple{
    Fields::Subclass{.Marker = "AcDb3PointAngularDimension"},
    Fields::Field{.Code = 13, .Member = &Entities::Forms::Points::Start},
    Fields::Field{.Code = 14, .Member = &Entities::Forms::Points::End},
    Fields::Field{.Code = 15, .Member = &Entities::Forms::Points::Vertex},
};

template <>
inline constexpr auto Shapes<Entities::Forms::Ordinate> = std::tuple{
    Fields::Subclass{.Marker = "AcDbOrdinateDimension"},
    Fields::Field{.Code = 13, .Member = &Entities::Forms::Ordinate::Feature},
    Fields::Field{.Code = 14, .Member = &Entities::Forms::Ordinate::Leader},
};
} // namespace DxfIO

#endif /* DXFIO_ENTITIES_SHAPES_DIMENSION_FORMS_HPP */
