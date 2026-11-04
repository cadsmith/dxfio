#ifndef DXFIO_UNKNOWN_HPP
#define DXFIO_UNKNOWN_HPP

#include "dxfio/api.hpp"
#include "dxfio/base.hpp"
#include "dxfio/group.hpp"
#include "dxfio/symbol.hpp"

#include <vector>

namespace DxfIO
{
/// A record of a kind dxfio has no type for, or one that did not fit its kind's layout, kept whole to be written
/// back as it came.
struct DXFIO_API Unknown
{
  /// The DXF name its 0 group gives, such as "CAMERA", kept for as long as the program runs.
  Symbol Type{};
  /// Its handle, owner, reactors, extension dictionary and extended data, read as for any record.
  DxfIO::Base Base{};
  /// Every other group, subclass markers included, in the order the file gave them.
  std::vector<Group> Groups{};

  auto operator==(const Unknown &) const -> bool = default;
};

} // namespace DxfIO

#endif /* DXFIO_UNKNOWN_HPP */
