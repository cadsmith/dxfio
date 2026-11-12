#ifndef DXFIO_BLOCKS_SHAPES_END_HPP
#define DXFIO_BLOCKS_SHAPES_END_HPP

#include "../../entities/shapes/record.hpp"
#include "fields/subclass.hpp"
#include "shapes.hpp"

#include "dxfio/blocks/end.hpp"

#include <string_view>
#include <tuple>

namespace DxfIO
{
  template <>
  inline constexpr auto Shapes<Blocks::End> = std::tuple{
      Entities::Part<Blocks::End>,
      Fields::Subclass{.Marker = "AcDbBlockEnd"},
  };

  template <>
  inline constexpr std::string_view Type<Blocks::End> = "ENDBLK";
} // namespace DxfIO

#endif /* DXFIO_BLOCKS_SHAPES_END_HPP */
