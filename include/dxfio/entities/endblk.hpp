#ifndef DXFIO_ENTITIES_ENDBLK_HPP
#define DXFIO_ENTITIES_ENDBLK_HPP

#include "dxfio/api.hpp"
#include "dxfio/entities/record.hpp"

namespace DxfIO::Entities
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
} // namespace DxfIO::Entities

#endif /* DXFIO_ENTITIES_ENDBLK_HPP */
