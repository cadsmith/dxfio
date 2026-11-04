#ifndef DXFIO_OBJECTS_SHAPES_PLOT_HPP
#define DXFIO_OBJECTS_SHAPES_PLOT_HPP

#include "fields/field.hpp"
#include "fields/numbers.hpp"
#include "fields/part.hpp"
#include "fields/since.hpp"
#include "fields/subclass.hpp"
#include "fields/text.hpp"
#include "shapes.hpp"

#include "dxfio/objects/plot.hpp"
#include "dxfio/vector.hpp"
#include "dxfio/version.hpp"

#include <tuple>

namespace DxfIO
{
namespace Objects
{
template <int Across, int Up>
inline constexpr auto Pair = std::tuple{
    Fields::Field<Vector, double>{.Code = Across, .Member = &Vector::X},
    Fields::Field<Vector, double>{.Code = Up, .Member = &Vector::Y},
};
} // namespace Objects

template <>
inline constexpr auto Shapes<Objects::Plot> =
    std::tuple{
        Fields::Subclass{.Marker = "AcDbPlotSettings"},
        Fields::Field{.Code = 1, .Member = &Objects::Plot::Name},
        Fields::Field{.Code = 2, .Member = &Objects::Plot::Device},
        Fields::Field{.Code = 4, .Member = &Objects::Plot::Size},
        Fields::Field{.Code = 6, .Member = &Objects::Plot::View},
        Fields::Field{.Code = 40, .Member = &Objects::Plot::Left},
        Fields::Field{.Code = 41, .Member = &Objects::Plot::Bottom},
        Fields::Field{.Code = 42, .Member = &Objects::Plot::Right},
        Fields::Field{.Code = 43, .Member = &Objects::Plot::Top},
        Fields::Field{.Code = 44, .Member = &Objects::Plot::Width},
        Fields::Field{.Code = 45, .Member = &Objects::Plot::Height},
        Fields::Part{
            .Member = &Objects::Plot::Origin,
            .Fields = Objects::Pair<46, 47>,
        },
        Fields::Part{
            .Member = &Objects::Plot::Lower,
            .Fields = Objects::Pair<48, 49>,
        },
        Fields::Part{
            .Member = &Objects::Plot::Upper,
            .Fields = Objects::Pair<140, 141>,
        },
        Fields::Field{.Code = 142, .Member = &Objects::Plot::Paper},
        Fields::Field{.Code = 143, .Member = &Objects::Plot::Drawing},
        Fields::Field{.Code = 70, .Member = &Objects::Plot::Flags},
        Fields::Field{.Code = 72, .Member = &Objects::Plot::Units},
        Fields::Field{.Code = 73, .Member = &Objects::Plot::Rotation},
        Fields::Field{.Code = 74, .Member = &Objects::Plot::Type},
        Fields::Field{.Code = 7, .Member = &Objects::Plot::Style},
        Fields::Field{.Code = 75, .Member = &Objects::Plot::Standard},
        Fields::Field{.Code = 147, .Member = &Objects::Plot::Factor},
        Fields::Since{
            .Release = Version::R2004,
            .Fields =
                std::tuple{
                    Fields::Field{
                        .Code = 76,
                        .Member = &Objects::Plot::Shading,
                    },
                    Fields::Field{
                        .Code = 77,
                        .Member = &Objects::Plot::Resolution,
                    },
                    Fields::Field{
                        .Code = 78,
                        .Member = &Objects::Plot::Dots,
                    },
                },
        },
        Fields::Part{
            .Member = &Objects::Plot::Image,
            .Fields = Objects::Pair<148, 149>,
        },
};
} // namespace DxfIO

#endif /* DXFIO_OBJECTS_SHAPES_PLOT_HPP */
