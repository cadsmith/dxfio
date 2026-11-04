#ifndef DXFIO_RAW_HPP
#define DXFIO_RAW_HPP

#include "dxfio/api.hpp"
#include "dxfio/group.hpp"

#include <string>
#include <vector>

namespace DxfIO
{
/// A record kept as its groups alone, as each CLASS of the CLASSES section is.
struct DXFIO_API Raw
{
  /// The name its 0 group gives, such as "CLASS".
  std::string Type;
  /// The groups after the 0, in their order.
  std::vector<Group> Groups;

  auto operator==(const Raw &) const -> bool = default;
};

} // namespace DxfIO

#endif /* DXFIO_RAW_HPP */
