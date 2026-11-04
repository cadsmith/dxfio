#ifndef DXFIO_ENTITIES_SHAPES_SEQEND_HPP
#define DXFIO_ENTITIES_SHAPES_SEQEND_HPP

#include "record.hpp"
#include "shapes.hpp"

#include "dxfio/entities/seqend.hpp"

#include <string_view>
#include <tuple>

namespace DxfIO
{
template <>
inline constexpr auto Shapes<Entities::Seqend> = std::tuple{Entities::Part<Entities::Seqend>};

template <>
inline constexpr std::string_view Type<Entities::Seqend> = "SEQEND";
} // namespace DxfIO

#endif /* DXFIO_ENTITIES_SHAPES_SEQEND_HPP */
