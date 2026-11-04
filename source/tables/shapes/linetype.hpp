#ifndef DXFIO_TABLES_SHAPES_LINETYPE_HPP
#define DXFIO_TABLES_SHAPES_LINETYPE_HPP

#include "fields/count.hpp"
#include "fields/each.hpp"
#include "fields/handles.hpp"
#include "fields/part.hpp"
#include "fields/since.hpp"
#include "fields/when.hpp"
#include "masked.hpp"
#include "record.hpp"

#include "dxfio/tables/ltype.hpp"

#include <string_view>

namespace DxfIO
{
namespace Tables
{
inline auto Embeds(const Dash &one) -> bool
{
  constexpr int Embedded = 6;
  return Masked(one.Flags, Embedded) != 0;
}

inline auto Letters(const Dash &one) -> bool
{
  constexpr int Lettered = 2;
  return Masked(one.Flags, Lettered) != 0;
}

inline constexpr auto Figure = std::tuple{
    Fields::Field{.Code = 75, .Member = &Dash::Shape}, Fields::Field{.Code = 340, .Member = &Dash::Style},
    Fields::Field{.Code = 46, .Member = &Dash::Scale}, Fields::Field{.Code = 50, .Member = &Dash::Rotation},
    Fields::Field{.Code = 44, .Member = &Dash::Along}, Fields::Field{.Code = 45, .Member = &Dash::Across},
};
} // namespace Tables

template <>
inline constexpr auto Shapes<Tables::Dash> = std::tuple{
    Fields::Field{.Code = 49, .Member = &Tables::Dash::Length},
    Fields::Since{
        .Release = Version::R13,
        .Fields =
            std::tuple{
                Fields::Field{.Code = 74, .Member = &Tables::Dash::Flags},
                Fields::When{.Test = Tables::Embeds, .Fields = Tables::Figure},
                Fields::When{
                    .Test = Tables::Letters,
                    .Fields = std::tuple{Fields::Field{
                        .Code = 9,
                        .Member = &Tables::Dash::Text,
                    }},
                },
            },
    },
};

template <>
inline constexpr auto Shapes<Tables::Ltype> = std::tuple{
    Tables::Opening,
    Fields::Subclass{.Marker = "AcDbLinetypeTableRecord"},
    Fields::Part{
        .Member = &Tables::Ltype::Record,
        .Fields = Shapes<Tables::Record>,
    },
    Fields::Field{.Code = 3, .Member = &Tables::Ltype::Description},
    Fields::Field{.Code = 72, .Member = &Tables::Ltype::Alignment},
    Fields::Count{.Code = 73, .Member = &Tables::Ltype::Dashes},
    Fields::Field{.Code = 40, .Member = &Tables::Ltype::Length},
    Fields::Each{
        .Opener = 49,
        .Member = &Tables::Ltype::Dashes,
        .Fields = Shapes<Tables::Dash>,
    },
};

template <>
inline constexpr std::string_view Type<Tables::Ltype> = "LTYPE";
} // namespace DxfIO

#endif /* DXFIO_TABLES_SHAPES_LINETYPE_HPP */
