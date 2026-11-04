#ifndef DXFIO_BASE_HPP
#define DXFIO_BASE_HPP

#include "dxfio/api.hpp"
#include "dxfio/brace.hpp"
#include "dxfio/group.hpp"
#include "dxfio/handle.hpp"
#include "dxfio/xdata.hpp"

#include <vector>

namespace DxfIO
{
/// What every record carries besides its own fields: its handle, owner, reactors, extension dictionary,
/// extended data, and the groups its kind does not name.
struct DXFIO_API Base
{
  /// The record's handle, group 5 (105 in a DIMSTYLE); null when the file gave it none.
  Handle Self{};
  /// The handle of the record that owns it, group 330, from R13 on.
  Handle Owner{};
  /// The handle of its extension dictionary, the 360 group in its 102 {ACAD_XDICTIONARY braces; null when none.
  Handle Dictionary{};
  /// The handles of the objects that watch it, the 330 groups in its 102 {ACAD_REACTORS braces.
  std::vector<Handle> Reactors{};
  /// Its other 102 braces, each with its name and the groups inside it, in the order the file gave them.
  std::vector<Brace> Braces{};
  /// Its extended data, one entry for each 1001 group at the end of the record.
  std::vector<Xdata> Extended{};
  /// The groups its kind does not name, written back after its own fields when saved to the release they came in.
  std::vector<Group> Extra{};

  auto operator==(const Base &) const -> bool = default;
};
} // namespace DxfIO

#endif /* DXFIO_BASE_HPP */
