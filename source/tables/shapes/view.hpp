#ifndef DXFIO_TABLES_SHAPES_VIEW_HPP
#define DXFIO_TABLES_SHAPES_VIEW_HPP

#include "axes.hpp"
#include "fields/flat.hpp"
#include "fields/part.hpp"
#include "fields/since.hpp"
#include "fields/when.hpp"
#include "record.hpp"

#include "dxfio/tables/view.hpp"

#include <string_view>

namespace DxfIO
{
namespace Tables
{
inline auto Associates(const View &one) -> bool
{
  return one.Associated;
}

inline constexpr auto Associated = std::tuple{
    Fields::Field{.Code = 281, .Member = &View::Render},
    Fields::Field{.Code = 72, .Member = &View::Associated},
    Fields::When{
        .Test = Associates,
        .Fields = std::tuple{Fields::Part{
            .Member = &View::Axes,
            .Fields = Shapes<Axes>,
        }},
    },
};

inline constexpr auto Camera = std::tuple{
    Fields::Field{.Code = 73, .Member = &View::Plottable},
    Fields::Field{.Code = 332, .Member = &View::Background, .Fallback = Handle{}},
    Fields::Field{.Code = 334, .Member = &View::Section, .Fallback = Handle{}},
    Fields::Field{.Code = 348, .Member = &View::Style, .Fallback = Handle{}},
};
} // namespace Tables

template <>
inline constexpr auto Shapes<Tables::View> = std::tuple{
    Tables::Opening,
    Fields::Subclass{.Marker = "AcDbViewTableRecord"},
    Fields::Part{
        .Member = &Tables::View::Record,
        .Fields = Shapes<Tables::Record>,
    },
    Fields::Field{.Code = 40, .Member = &Tables::View::Height},
    Fields::Flat{.Code = 10, .Member = &Tables::View::Center},
    Fields::Field{.Code = 41, .Member = &Tables::View::Width},
    Fields::Field{.Code = 11, .Member = &Tables::View::Direction},
    Fields::Field{.Code = 12, .Member = &Tables::View::Target},
    Fields::Field{.Code = 42, .Member = &Tables::View::Focal},
    Fields::Field{.Code = 43, .Member = &Tables::View::Front},
    Fields::Field{.Code = 44, .Member = &Tables::View::Back},
    Fields::Field{.Code = 50, .Member = &Tables::View::Twist},
    Fields::Field{.Code = 71, .Member = &Tables::View::Mode},
    Fields::Since{.Release = Version::R2000, .Fields = Tables::Associated},
    Fields::Since{.Release = Version::R2007, .Fields = Tables::Camera},
};

template <>
inline constexpr std::string_view Type<Tables::View> = "VIEW";
} // namespace DxfIO

#endif /* DXFIO_TABLES_SHAPES_VIEW_HPP */
