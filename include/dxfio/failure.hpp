#ifndef DXFIO_FAILURE_HPP
#define DXFIO_FAILURE_HPP

#include "dxfio/api.hpp"

#include <cstddef>
#include <string>

namespace DxfIO
{
/// Why a drawing could not be read or written, or an edit could not be made.
struct DXFIO_API Failure
{
  /// The line of the file where it went wrong, counting from 1; zero when no line is to blame, as in a binary file.
  std::size_t Line{};
  /// What went wrong, in a sentence such as "ENDSEC of TABLES missing".
  std::string What;

  auto operator==(const Failure &) const -> bool = default;
};
} // namespace DxfIO

#endif /* DXFIO_FAILURE_HPP */
