#ifndef DXFIO_ISSUE_HPP
#define DXFIO_ISSUE_HPP

#include "dxfio/api.hpp"
#include "dxfio/handle.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>

namespace DxfIO
{
  /**
   *  What kind of thing the reader or writer noted.
   */
  enum class IssueCode : std::uint8_t
  {
    /**
     *  Structure the reader found broken and stepped over or mended.
     */
    Damaged,
    /**
     *  A value or a kind dxfio does not know, kept or skipped.
     */
    Unknown,
    /**
     *  A value the writer could not write as it is, changed or cut.
     */
    OutOfRange,
    /**
     *  What the release saved cannot hold, left out or written otherwise.
     */
    NotKept,
  };

  /**
   *  Something the reader or writer had to note: what it kept, mended or left out, and where.
   */
  struct DXFIO_API Issue
  {
    /**
     *  What kind of issue it is, to act on without reading What.
     */
    IssueCode Code{};
    /**
     *  The line of the file it was found on, counting from 1, when one is to blame; none in a save
     *  or a binary file.
     */
    std::optional<std::size_t> Line{};
    /**
     *  The handle of the record it concerns; null when it concerns no one record, or the record has
     *  no handle.
     */
    Handle Subject{};
    /**
     *  What was noted, in a short phrase such as "group 330 given again, one kept".
     */
    std::string What{};

    auto operator==(const Issue &) const -> bool = default;
  };
} // namespace DxfIO

#endif /* DXFIO_ISSUE_HPP */
