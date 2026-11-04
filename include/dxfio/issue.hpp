#ifndef DXFIO_ISSUE_HPP
#define DXFIO_ISSUE_HPP

#include "dxfio/api.hpp"
#include "dxfio/handle.hpp"

#include <cstddef>
#include <string>

namespace DxfIO
{
/// Something the reader or writer had to note: what it kept, mended or left out, and where.
struct DXFIO_API Issue
{
  /// The line of the file it was found on, counting from 1; zero when no line is to blame, as in a save.
  std::size_t Line{};
  /// The handle of the record it concerns; null when it concerns no one record, or the record has no handle.
  Handle Record{};
  /// What was noted, in a sentence such as "group 330 given again, one kept".
  std::string What;

  auto operator==(const Issue &) const -> bool = default;
};
} // namespace DxfIO

#endif /* DXFIO_ISSUE_HPP */
