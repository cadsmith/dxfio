#ifndef DXFIO_BLOCKS_BEGIN_HPP
#define DXFIO_BLOCKS_BEGIN_HPP

#include "dxfio/api.hpp"
#include "dxfio/entities/record.hpp"
#include "dxfio/vector.hpp"

#include <string>

namespace DxfIO::Blocks
{
/// A BLOCK: the record that opens a block, with its name and the base point inserts place it by.
struct DXFIO_API Begin
{
  /// The layer, linetype, color and the rest every entity carries.
  Entities::Record Record{};
  /// The name of the block. Group 2, written again as group 3.
  std::string Name;
  /// Bits: 1 anonymous, 2 with attribute definitions that are not constant, 4 an xref, 8 an xref overlay,
  /// 16 from an xref, 32 a resolved xref, 64 referenced. Group 70.
  int Flags{};
  /// The base point, which an insert places at its own insertion point. Groups 10, 20, 30.
  Vector Origin{};
  /// The path of the xref file; empty for a block of the drawing's own. Group 1.
  std::string Path;
  /// The block's description. Group 4, from R2000.
  std::string Description;

  auto operator==(const Begin &) const -> bool = default;
};
} // namespace DxfIO::Blocks

#endif /* DXFIO_BLOCKS_BEGIN_HPP */
