#ifndef DXFIO_OBJECTS_XRECORD_HPP
#define DXFIO_OBJECTS_XRECORD_HPP

#include "dxfio/api.hpp"
#include "dxfio/base.hpp"
#include "dxfio/group.hpp"

#include <vector>

namespace DxfIO::Objects
{
/// An XRECORD: groups an application keeps with no meaning to AutoCAD.
struct DXFIO_API XRecord
{
  /// The handle, owner, reactors and extended data.
  DxfIO::Base Base{};
  /// What cloning does with a name already taken, as a dictionary's. Group 280, the first.
  int Cloning{1};
  /// The application's groups, in the order they came.
  std::vector<DxfIO::Group> Data;

  auto operator==(const XRecord &) const -> bool = default;
};
} // namespace DxfIO::Objects

#endif /* DXFIO_OBJECTS_XRECORD_HPP */
