#ifndef DXFIO_ENTITIES_SHAPES_DIMENSION_DIMENSION_HPP
#define DXFIO_ENTITIES_SHAPES_DIMENSION_DIMENSION_HPP

#include "../../../variants.hpp"
#include "../record.hpp"
#include "fields/custom.hpp"
#include "fields/field.hpp"
#include "fields/numbers.hpp"
#include "fields/part.hpp"
#include "fields/picture.hpp"
#include "fields/points.hpp"
#include "fields/subclass.hpp"
#include "forms.hpp"
#include "masked.hpp"
#include "measure.hpp"
#include "shapes.hpp"

#include "dxfio/entities/dimension/dimension.hpp"

#include <cstddef>
#include <expected>
#include <string_view>
#include <tuple>

namespace DxfIO
{
namespace Entities
{
inline constexpr int Formal = 7;

inline void Shaped(Figure &one, Fields::Walk &in)
{
  Fields::Read(one.Measure, in, Shapes<Measure>);
  Become(one.Form, static_cast<std::size_t>(Masked(one.Measure.Flags, Formal)), in);
}

inline auto Shaping(const Figure &one, Fields::Writing &to) -> std::expected<void, Failure>
{
  Measure kind = one.Measure;
  kind.Flags += static_cast<int>(one.Form.index()) - Masked(kind.Flags, Formal);
  return Fields::Write(kind, to, Shapes<Measure>).and_then([&] -> auto { return Spoken(one.Form, to); });
}
} // namespace Entities

template <>
inline constexpr auto Shapes<Entities::Figure> = std::tuple{
    Fields::Custom<Entities::Figure>{
        .Read = Entities::Shaped,
        .Write = Entities::Shaping,
    },
};

template <>
inline constexpr auto Shapes<Entities::Bend> = std::tuple{
    Fields::Part{
        .Member = &Entities::Bend::Measure,
        .Fields = Shapes<Entities::Measure>,
    },
    Fields::Subclass{.Marker = "AcDbArcDimension"},
    Fields::Field{.Code = 13, .Member = &Entities::Bend::Start},
    Fields::Field{.Code = 14, .Member = &Entities::Bend::End},
    Fields::Field{.Code = 15, .Member = &Entities::Bend::Center},
    Fields::Field{.Code = 70, .Member = &Entities::Bend::Partial},
    Fields::Field{.Code = 40, .Member = &Entities::Bend::From},
    Fields::Field{.Code = 41, .Member = &Entities::Bend::To},
    Fields::Field{.Code = 71, .Member = &Entities::Bend::Led},
    Fields::Field{.Code = 16, .Member = &Entities::Bend::Near},
    Fields::Field{.Code = 17, .Member = &Entities::Bend::Far},
};

template <>
inline constexpr auto Shapes<Entities::Jog> = std::tuple{
    Fields::Part{
        .Member = &Entities::Jog::Measure,
        .Fields = Shapes<Entities::Measure>,
    },
    Fields::Subclass{.Marker = "AcDbRadialDimensionLarge"},
    Fields::Field{.Code = 13, .Member = &Entities::Jog::Chord},
    Fields::Field{.Code = 14, .Member = &Entities::Jog::Center},
    Fields::Field{.Code = 15, .Member = &Entities::Jog::Turn},
    Fields::Field{.Code = 40, .Member = &Entities::Jog::Angle},
};

template <>
inline constexpr auto Shapes<Entities::Dimension> = std::tuple{
    Entities::Part<Entities::Dimension>,
    Fields::Part{
        .Member = &Entities::Dimension::Figure,
        .Fields = Shapes<Entities::Figure>,
    },
};

template <>
inline constexpr auto Shapes<Entities::ArcDimension> = std::tuple{
    Entities::Part<Entities::ArcDimension>,
    Fields::Picture{.Member = &Entities::ArcDimension::Picture},
    Fields::Part{
        .Member = &Entities::ArcDimension::Bend,
        .Fields = Shapes<Entities::Bend>,
    },
};

template <>
inline constexpr auto Shapes<Entities::LargeRadialDimension> = std::tuple{
    Entities::Part<Entities::LargeRadialDimension>,
    Fields::Picture{.Member = &Entities::LargeRadialDimension::Picture},
    Fields::Part{
        .Member = &Entities::LargeRadialDimension::Jog,
        .Fields = Shapes<Entities::Jog>,
    },
};

template <>
inline constexpr std::string_view Type<Entities::Dimension> = "DIMENSION";

template <>
inline constexpr std::string_view Type<Entities::ArcDimension> = "ARC_DIMENSION";

template <>
inline constexpr std::string_view Type<Entities::LargeRadialDimension> = "LARGE_RADIAL_DIMENSION";
} // namespace DxfIO

#endif /* DXFIO_ENTITIES_SHAPES_DIMENSION_DIMENSION_HPP */
