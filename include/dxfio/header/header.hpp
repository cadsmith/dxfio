#ifndef DXFIO_HEADER_HEADER_HPP
#define DXFIO_HEADER_HEADER_HPP

#include "dxfio/api.hpp"
#include "dxfio/group.hpp"

#include <string>
#include <vector>

namespace DxfIO::Header
{
/// A variable of the HEADER section: a drawing setting such as $INSBASE, and the groups that hold its value.
struct DXFIO_API Variable
{
  /// The name, with its leading $, such as "$ACADVER". Group 9.
  std::string Name{};
  /// The groups of its value, as they follow the name: one group 1 for $ACADVER, groups 10, 20, 30 for a point.
  std::vector<Group> Values{};

  auto operator==(const Variable &) const -> bool = default;
};

} // namespace DxfIO::Header

#endif /* DXFIO_HEADER_HEADER_HPP */
