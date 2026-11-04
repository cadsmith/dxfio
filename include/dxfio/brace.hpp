#ifndef DXFIO_BRACE_HPP
#define DXFIO_BRACE_HPP

#include "dxfio/api.hpp"
#include "dxfio/group.hpp"

#include <string>
#include <vector>

namespace DxfIO
{
/// A 102 brace group of a record, from {NAME to }, in which an application keeps groups of its own.
/// The {ACAD_REACTORS and {ACAD_XDICTIONARY braces are not kept as one: Base holds what they point to.
struct DXFIO_API Brace
{
  /// The name after the opening brace, such as "BLKREFS".
  std::string Name;
  /// The groups between the opening and the closing 102, in their order.
  std::vector<Group> Groups;

  auto operator==(const Brace &) const -> bool = default;
};
} // namespace DxfIO

#endif /* DXFIO_BRACE_HPP */
