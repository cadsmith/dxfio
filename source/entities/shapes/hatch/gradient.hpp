#ifndef DXFIO_ENTITIES_SHAPES_HATCH_GRADIENT_HPP
#define DXFIO_ENTITIES_SHAPES_HATCH_GRADIENT_HPP

#include "fields/count.hpp"
#include "fields/each.hpp"
#include "fields/field.hpp"
#include "fields/numbers.hpp"
#include "fields/optional.hpp"
#include "fields/text.hpp"
#include "shapes.hpp"

#include "dxfio/entities/hatch/gradient.hpp"

#include <tuple>

namespace DxfIO
{
template <>
inline constexpr auto Shapes<Entities::Stop> = std::tuple{
    Fields::Field{.Code = 463, .Member = &Entities::Stop::Place},
    Fields::Field{.Code = 63, .Member = &Entities::Stop::Color},
    Fields::Field{.Code = 421, .Member = &Entities::Stop::Rgb},
};

template <>
inline constexpr auto Shapes<Entities::Gradient> = std::tuple{
    Fields::Field{.Code = 450, .Member = &Entities::Gradient::Shown},
    Fields::Field{.Code = 451, .Member = &Entities::Gradient::Reserved},
    Fields::Field{.Code = 460, .Member = &Entities::Gradient::Angle},
    Fields::Field{.Code = 461, .Member = &Entities::Gradient::Shift},
    Fields::Field{.Code = 452, .Member = &Entities::Gradient::Single},
    Fields::Field{.Code = 462, .Member = &Entities::Gradient::Tint},
    Fields::Count{.Code = 453, .Member = &Entities::Gradient::Stops},
    Fields::Each{
        .Opener = 463,
        .Member = &Entities::Gradient::Stops,
        .Fields = Shapes<Entities::Stop>,
    },
    Fields::Field{.Code = 470, .Member = &Entities::Gradient::Name},
};
} // namespace DxfIO

#endif /* DXFIO_ENTITIES_SHAPES_HATCH_GRADIENT_HPP */
