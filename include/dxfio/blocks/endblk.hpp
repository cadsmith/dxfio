#ifndef DXFIO_BLOCKS_ENDBLK_HPP
#define DXFIO_BLOCKS_ENDBLK_HPP

#include "dxfio/api.hpp"
#include "dxfio/entities/record.hpp"

namespace DxfIO::Blocks
{
  /**
   *  An ENDBLK: the record that closes a block.
   */
  struct DXFIO_API EndBlk
  {
    /**
     *  The layer, linetype, color and the rest every entity carries.
     */
    Entities::Record Record{};

    auto operator==(const EndBlk &) const -> bool = default;
  };
} // namespace DxfIO::Blocks

#endif /* DXFIO_BLOCKS_ENDBLK_HPP */
