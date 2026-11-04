#ifndef DXFIO_TABLES_SHAPES_STYLE_HPP
#define DXFIO_TABLES_SHAPES_STYLE_HPP

#include "fields/part.hpp"
#include "record.hpp"

#include "dxfio/tables/style.hpp"

#include <string_view>

namespace DxfIO
{
template <>
inline constexpr auto Shapes<Tables::Style> = std::tuple{
    Tables::Opening,
    Fields::Subclass{.Marker = "AcDbTextStyleTableRecord"},
    Fields::Part{
        .Member = &Tables::Style::Record,
        .Fields = Shapes<Tables::Record>,
    },
    Fields::Field{.Code = 40, .Member = &Tables::Style::Height},
    Fields::Field{.Code = 41, .Member = &Tables::Style::Width},
    Fields::Field{.Code = 50, .Member = &Tables::Style::Oblique},
    Fields::Field{.Code = 71, .Member = &Tables::Style::Generation},
    Fields::Field{.Code = 42, .Member = &Tables::Style::Previous},
    Fields::Field{.Code = 3, .Member = &Tables::Style::Font},
    Fields::Field{.Code = 4, .Member = &Tables::Style::Bigfont},
};

template <>
inline constexpr std::string_view Type<Tables::Style> = "STYLE";
} // namespace DxfIO

#endif /* DXFIO_TABLES_SHAPES_STYLE_HPP */
