#ifndef DXFIO_ENTITIES_SHAPES_LETTERING_HPP
#define DXFIO_ENTITIES_SHAPES_LETTERING_HPP

#include "fields/field.hpp"
#include "fields/joined.hpp"
#include "fields/numbers.hpp"
#include "fields/optional.hpp"
#include "fields/part.hpp"
#include "fields/points.hpp"
#include "fields/text.hpp"
#include "shapes.hpp"

#include "dxfio/entities/lettering.hpp"
#include "dxfio/vector.hpp"

#include <string_view>
#include <tuple>
#include <type_traits>

namespace DxfIO
{
template <>
inline constexpr auto Shapes<Entities::Lettering> = std::tuple{
    Fields::Joined{.Marker = "AcDbText"},
    Fields::Field{
        .Code = 39,
        .Member = &Entities::Lettering::Thickness,
        .Fallback = 0.0,
    },
    Fields::Field{.Code = 10, .Member = &Entities::Lettering::Position},
    Fields::Field{.Code = 40, .Member = &Entities::Lettering::Height},
    Fields::Field{.Code = 1, .Member = &Entities::Lettering::Value},
    Fields::Field{
        .Code = 50,
        .Member = &Entities::Lettering::Rotation,
        .Fallback = 0.0,
    },
    Fields::Field{
        .Code = 41,
        .Member = &Entities::Lettering::Width,
        .Fallback = 1.0,
    },
    Fields::Field{
        .Code = 51,
        .Member = &Entities::Lettering::Oblique,
        .Fallback = 0.0,
    },
    Fields::Field{
        .Code = 7,
        .Member = &Entities::Lettering::Style,
        .Fallback = std::string_view{"Standard"},
    },
    Fields::Field{
        .Code = 71,
        .Member = &Entities::Lettering::Generation,
        .Fallback = 0,
    },
    Fields::Field{
        .Code = 72,
        .Member = &Entities::Lettering::Horizontal,
        .Fallback = 0,
    },
    Fields::Field{.Code = 11, .Member = &Entities::Lettering::Alignment},
    Fields::Field{
        .Code = 210,
        .Member = &Entities::Lettering::Extrusion,
        .Fallback = Up,
    },
};

namespace Entities
{
template <class KindT>
using Letters = Fields::Part<KindT, decltype(KindT::Lettering), std::remove_const_t<decltype(Shapes<Lettering>)>>;

template <class KindT>
inline constexpr Letters<KindT> Lettered{
    .Member = &KindT::Lettering,
    .Fields = Shapes<Lettering>,
};
} // namespace Entities
} // namespace DxfIO

#endif /* DXFIO_ENTITIES_SHAPES_LETTERING_HPP */
