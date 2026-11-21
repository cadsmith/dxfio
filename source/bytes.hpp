#ifndef DXFIO_BYTES_HPP
#define DXFIO_BYTES_HPP

#include <cstddef>
#include <span>
#include <string_view>
#include <vector>

namespace DxfIO
{
  /**
   *  The bytes as the characters a tag carries them in, valid as long as the bytes are.
   */
  inline auto Viewed(std::span<const std::byte> bytes) -> std::string_view
  {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast): a char may alias any byte
    return {reinterpret_cast<const char *>(bytes.data()), bytes.size()};
  }

  /**
   *  Adds the characters of a tag to the bytes, as they are.
   */
  inline void Append(std::vector<std::byte> &out, std::string_view piece)
  {
    const std::span<const std::byte> bytes = std::as_bytes(std::span{piece});
    out.insert(out.end(), bytes.begin(), bytes.end());
  }
} // namespace DxfIO

#endif /* DXFIO_BYTES_HPP */
