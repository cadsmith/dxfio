#ifndef DXFIO_OBJECTS_GROUP_HPP
#define DXFIO_OBJECTS_GROUP_HPP

#include "dxfio/api.hpp"
#include "dxfio/base.hpp"
#include "dxfio/handle.hpp"

#include <string>
#include <vector>

namespace DxfIO::Objects
{
/// A GROUP: entities picked together, named by its entry in the group dictionary.
struct DXFIO_API Group
{
  /// The handle, owner, reactors and extended data.
  DxfIO::Base Base{};
  /// The group's description. Group 300.
  std::string Description;
  /// 1 when the group has no name of its own, 0 when it is named. Group 70.
  int Unnamed{};
  /// 1 when picking one entity picks the whole group. Group 71.
  int Selectable{1};
  /// The entities in the group. Group 340, once each.
  std::vector<Handle> Members;

  auto operator==(const Group &) const -> bool = default;
};
} // namespace DxfIO::Objects

#endif /* DXFIO_OBJECTS_GROUP_HPP */
