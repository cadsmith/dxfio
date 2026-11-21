#ifndef DXFIO_BLOCKS_SHAPES_ENDBLK_HPP
#define DXFIO_BLOCKS_SHAPES_ENDBLK_HPP

#include "../../entities/shapes/record.hpp"
#include "fields/subclass.hpp"
#include "shapes.hpp"

#include "dxfio/blocks/endblk.hpp"

#include <string_view>
#include <tuple>

namespace DxfIO
{
  template <>
  inline constexpr auto Shapes<Blocks::EndBlk> = std::tuple{
      Entities::Part<Blocks::EndBlk>,
      Fields::Subclass{.Marker = "AcDbBlockEnd"},
  };

  template <>
  inline constexpr std::string_view Type<Blocks::EndBlk> = "ENDBLK";
} // namespace DxfIO

#endif /* DXFIO_BLOCKS_SHAPES_ENDBLK_HPP */
