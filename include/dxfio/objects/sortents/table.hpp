#ifndef DXFIO_OBJECTS_SORTENTS_TABLE_HPP
#define DXFIO_OBJECTS_SORTENTS_TABLE_HPP

#include "dxfio/api.hpp"
#include "dxfio/base.hpp"
#include "dxfio/handle.hpp"

#include <optional>
#include <vector>

namespace DxfIO::Objects::Sortents
{
/// One entity whose place in the draw order is not its handle's.
struct DXFIO_API Entry
{
  /// The entity. Group 331.
  Handle Entity{};
  /// The handle the entity is drawn in the order of, in place of its own. Group 5.
  std::optional<Handle> Key;

  auto operator==(const Entry &) const -> bool = default;
};
} // namespace DxfIO::Objects::Sortents

namespace DxfIO::Objects
{
/// A SORTENTSTABLE: the draw order of a space's entities, where it differs from the order of their handles.
struct DXFIO_API SortEntsTable
{
  /// The handle, owner, reactors and extended data.
  DxfIO::Base Base{};
  /// The BLOCK_RECORD of the space whose entities it orders. Group 330.
  Handle Block{};
  /// The entities it moves in the draw order.
  std::vector<Sortents::Entry> Entries;

  auto operator==(const SortEntsTable &) const -> bool = default;
};
} // namespace DxfIO::Objects

#endif /* DXFIO_OBJECTS_SORTENTS_TABLE_HPP */
