#ifndef DXFIO_BLOCKS_BLOCKS_HPP
#define DXFIO_BLOCKS_BLOCKS_HPP

#include "dxfio/api.hpp"
#include "dxfio/blocks/begin.hpp"
#include "dxfio/blocks/end.hpp"
#include "dxfio/entity.hpp"

namespace DxfIO::Blocks
{
/// A block of the BLOCKS section: the entities between its BLOCK and its ENDBLK.
struct DXFIO_API Block
{
  /// The BLOCK that opens the block, with its name, flags and base point.
  Blocks::Begin Begin{};
  /// The entities of the block, in the order they are drawn.
  DxfIO::Entities::List Entities{};
  /// The ENDBLK that closes the block.
  Blocks::End End{};

  auto operator==(const Block &) const -> bool = default;
};

} // namespace DxfIO::Blocks

#endif /* DXFIO_BLOCKS_BLOCKS_HPP */
