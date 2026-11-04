#ifndef DXFIO_ENTITIES_SHAPES_ARC_HPP
#define DXFIO_ENTITIES_SHAPES_ARC_HPP

#include "fields/field.hpp"
#include "fields/numbers.hpp"
#include "fields/subclass.hpp"
#include "round.hpp"
#include "shapes.hpp"

#include "dxfio/entities/arc.hpp"

#include <string_view>
#include <tuple>

namespace DxfIO
{
template <>
inline constexpr auto Shapes<Entities::Arc> =
    std::tuple_cat(Entities::Round<Entities::Arc>, std::tuple{
                                                       Fields::Subclass{.Marker = "AcDbArc"},
                                                       Fields::Field{.Code = 50, .Member = &Entities::Arc::Start},
                                                       Fields::Field{.Code = 51, .Member = &Entities::Arc::End},
                                                   });

template <>
inline constexpr std::string_view Type<Entities::Arc> = "ARC";
} // namespace DxfIO

#endif /* DXFIO_ENTITIES_SHAPES_ARC_HPP */
