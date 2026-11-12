#ifndef DXFIO_HANDLE_HPP
#define DXFIO_HANDLE_HPP

#include "dxfio/api.hpp"

#include <compare>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <string_view>

namespace DxfIO
{
  /**
   *  The number a record is known by in a drawing, and that other records give to point to it.
   */
  struct DXFIO_API Handle
  {
    /**
     *  The number, which DXF writes in hex; zero names no record.
     */
    std::uint64_t Value{};

    /**
     *  Whether it names a record, that is, is not zero.
     */
    constexpr explicit operator bool() const { return Value != 0; }

    constexpr auto operator<=>(const Handle &) const = default;
  };

  /**
   *  The handle as DXF writes it: uppercase hex with no leading zeros, such as "1F", and "0" for
   *  the null handle.
   */
  [[nodiscard]] DXFIO_API auto Hex(Handle one) -> std::string;

  /**
   *  Reads a handle written in hex, in either case, with spaces or tabs around it; "0" gives the
   *  null handle. Empty when the text holds no digits, anything but hex digits, or more than 16 of
   *  them.
   */
  [[nodiscard]] DXFIO_API auto Parsed(std::string_view text) -> std::optional<Handle>;
} // namespace DxfIO

/**
 *  Hashes a handle by its number, so handles can key an unordered map or set.
 */
template <>
struct std::hash<DxfIO::Handle>
{
  auto operator()(DxfIO::Handle one) const noexcept -> std::size_t
  {
    return std::hash<std::uint64_t>{}(one.Value);
  }
};

#endif /* DXFIO_HANDLE_HPP */
