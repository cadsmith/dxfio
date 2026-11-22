#ifndef DXFIO_LOCATED_HPP
#define DXFIO_LOCATED_HPP

#include <cstddef>
#include <optional>

namespace DxfIO
{
  /**
   *  The line an issue or a failure names: none for 0, which a tag of a binary file holds.
   */
  inline auto Located(std::size_t line) -> std::optional<std::size_t>
  {
    return line == 0 ? std::nullopt : std::optional{line};
  }
} // namespace DxfIO

#endif /* DXFIO_LOCATED_HPP */
