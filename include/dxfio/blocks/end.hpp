#ifndef DXFIO_BLOCKS_END_HPP
#define DXFIO_BLOCKS_END_HPP

#include "dxfio/api.hpp"
#include "dxfio/entities/record.hpp"

namespace DxfIO::Blocks
{
/// An ENDBLK: the record that closes a block.
struct DXFIO_API End
{
  /// The layer, linetype, color and the rest every entity carries.
  Entities::Record Record{};

  auto operator==(const End &) const -> bool = default;
};
} // namespace DxfIO::Blocks

#endif /* DXFIO_BLOCKS_END_HPP */
