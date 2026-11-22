#ifndef DXFIO_BLOCKS_SHAPES_BLOCK_HPP
#define DXFIO_BLOCKS_SHAPES_BLOCK_HPP

#include "../../entities/shapes/record.hpp"
#include "fields/context.hpp"
#include "fields/custom.hpp"
#include "fields/field.hpp"
#include "fields/numbers.hpp"
#include "fields/points.hpp"
#include "fields/since.hpp"
#include "fields/subclass.hpp"
#include "fields/text.hpp"
#include "fields/walk.hpp"
#include "shapes.hpp"

#include "dxfio/blocks/block.hpp"
#include "dxfio/failure.hpp"
#include "dxfio/version.hpp"

#include <expected>
#include <string>
#include <string_view>
#include <tuple>

namespace DxfIO
{
  namespace Blocks
  {
    inline void Again(Block & /*unused*/, Fields::Walk &in)
    {
      static_cast<void>(in.Take(3));
    }

    inline auto Named(const Block &one, Fields::Writing &to) -> std::expected<void, Failure>
    {
      return Fields::Value<std::string>::Write(to, 3, one.Name);
    }
  } // namespace Blocks

  template <>
  inline constexpr auto Shapes<Blocks::Block> = std::tuple{
      Entities::Part<Blocks::Block>,
      Fields::Subclass{.Marker = "AcDbBlockBegin"},
      Fields::Field{.Code = 2, .Member = &Blocks::Block::Name},
      Fields::Field{.Code = 70, .Member = &Blocks::Block::Flags},
      Fields::Field{.Code = 10, .Member = &Blocks::Block::Origin},
      Fields::Custom<Blocks::Block>{
          .Read  = Blocks::Again,
          .Write = Blocks::Named,
      },
      Fields::Field{.Code = 1, .Member = &Blocks::Block::Path},
      Fields::Since{
          .Release = Version::R2000,
          .Fields  = std::tuple{Fields::Field{
              .Code     = 4,
              .Member   = &Blocks::Block::Description,
              .Fallback = std::string_view{},
          }},
      },
  };

  template <>
  inline constexpr std::string_view Type<Blocks::Block> = "BLOCK";
} // namespace DxfIO

#endif /* DXFIO_BLOCKS_SHAPES_BLOCK_HPP */
