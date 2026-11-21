#ifndef DXFIO_TESTS_BYTES_HPP
#define DXFIO_TESTS_BYTES_HPP

#include <cstddef>
#include <span>
#include <string_view>
#include <vector>

namespace DxfIO::Tests
{
  /**
   *  The bytes of a text, one for each character, for a literal such as "\xde\xad".
   */
  inline auto BytesOf(std::string_view text) -> std::vector<std::byte>
  {
    const std::span<const std::byte> bytes = std::as_bytes(std::span{text});
    return {bytes.begin(), bytes.end()};
  }
} // namespace DxfIO::Tests

#endif /* DXFIO_TESTS_BYTES_HPP */
