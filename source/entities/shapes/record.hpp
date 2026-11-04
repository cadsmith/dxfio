#ifndef DXFIO_ENTITIES_SHAPES_RECORD_HPP
#define DXFIO_ENTITIES_SHAPES_RECORD_HPP

#include "fields/field.hpp"
#include "fields/handles.hpp"
#include "fields/numbers.hpp"
#include "fields/optional.hpp"
#include "fields/part.hpp"
#include "fields/subclass.hpp"
#include "fields/text.hpp"
#include "shapes.hpp"

#include "dxfio/colors.hpp"
#include "dxfio/entities/record.hpp"
#include "dxfio/handle.hpp"
#include "dxfio/lineweights.hpp"
#include "dxfio/version.hpp"

#include <string_view>
#include <tuple>
#include <type_traits>

namespace DxfIO
{
template <>
inline constexpr auto Shapes<Entities::Record> = std::tuple{
    Fields::Subclass{.Marker = "AcDbEntity"},
    Fields::Field{
        .Code = 67,
        .Member = &Entities::Record::Paper,
        .Fallback = false,
    },
    Fields::Field{.Code = 8, .Member = &Entities::Record::Layer},
    Fields::Field{
        .Code = 6,
        .Member = &Entities::Record::Linetype,
        .Fallback = std::string_view{"ByLayer"},
    },
    Fields::Field{
        .Code = 347,
        .Member = &Entities::Record::Material,
        .Fallback = Handle{},
        .Since = Version::R2007,
    },
    Fields::Field{
        .Code = 62,
        .Member = &Entities::Record::Color,
        .Fallback = Colors::Bylayer,
    },
    Fields::Field{
        .Code = 420,
        .Member = &Entities::Record::Rgb,
        .Since = Version::R2004,
    },
    Fields::Field{
        .Code = 430,
        .Member = &Entities::Record::Book,
        .Since = Version::R2004,
    },
    Fields::Field{
        .Code = 440,
        .Member = &Entities::Record::Transparency,
        .Since = Version::R2004,
    },
    Fields::Field{
        .Code = 48,
        .Member = &Entities::Record::Scale,
        .Fallback = 1.0,
        .Since = Version::R13,
    },
    Fields::Field{
        .Code = 60,
        .Member = &Entities::Record::Hidden,
        .Fallback = false,
        .Since = Version::R13,
    },
    Fields::Field{
        .Code = 370,
        .Member = &Entities::Record::Lineweight,
        .Fallback = Lineweights::Bylayer,
        .Since = Version::R2000,
    },
    Fields::Field{
        .Code = 390,
        .Member = &Entities::Record::Style,
        .Fallback = Handle{},
        .Since = Version::R2000,
    },
    Fields::Field{
        .Code = 284,
        .Member = &Entities::Record::Shadow,
        .Fallback = 0,
        .Since = Version::R2007,
    },
};

namespace Entities
{
template <class KindT>
inline constexpr auto Part = Fields::Part<KindT, Record, std::remove_const_t<decltype(Shapes<Record>)>>{
    .Member = &KindT::Record,
    .Fields = Shapes<Record>,
};
} // namespace Entities
} // namespace DxfIO

#endif /* DXFIO_ENTITIES_SHAPES_RECORD_HPP */
