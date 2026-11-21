#ifndef DXFIO_FAILURE_HPP
#define DXFIO_FAILURE_HPP

#include "dxfio/api.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>

namespace DxfIO
{
  /**
   *  What kind of failure stopped a load, a save or an edit.
   */
  enum class FailureCode : std::uint8_t
  {
    /**
     *  A stream or a file that could not be read or written.
     */
    Io,
    /**
     *  A file whose structure is broken past reading.
     */
    Damaged,
    /**
     *  What an edit needs, such as a block of that name.
     */
    Missing,
    /**
     *  A name or a handle an edit would give is taken.
     */
    Exists,
    /**
     *  An edit or a save the drawing cannot take, such as a record with no name.
     */
    Refused,
  };

  /**
   *  Why a drawing could not be read or written, or an edit could not be made.
   */
  struct DXFIO_API Failure
  {
    FailureCode Code{};
    /**
     *  The line of the file where it went wrong, counting from 1, when one is to blame; none in a
     *  binary file.
     */
    std::optional<std::size_t> Line{};
    /**
     *  What went wrong, in a short phrase such as "ENDSEC of TABLES missing".
     */
    std::string What{};

    auto operator==(const Failure &) const -> bool = default;
  };
} // namespace DxfIO

#endif /* DXFIO_FAILURE_HPP */
