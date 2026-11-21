#ifndef DXFIO_BLOCKS_DEFINITION_HPP
#define DXFIO_BLOCKS_DEFINITION_HPP

#include "dxfio/api.hpp"
#include "dxfio/blocks/block.hpp"
#include "dxfio/blocks/endblk.hpp"
#include "dxfio/entity.hpp"

namespace DxfIO::Blocks
{
  /**
   *  A block of the BLOCKS section: the entities between its BLOCK and its ENDBLK.
   */
  struct DXFIO_API Definition
  {
    /**
     *  The BLOCK that opens the block, with its name, flags and base point.
     */
    Blocks::Block Block{};
    /**
     *  The entities of the block, in the order they are drawn.
     */
    DxfIO::Entities::List Entities{};
    /**
     *  The ENDBLK that closes the block.
     */
    Blocks::EndBlk EndBlk{};

    auto operator==(const Definition &) const -> bool = default;
  };

} // namespace DxfIO::Blocks

#endif /* DXFIO_BLOCKS_DEFINITION_HPP */
