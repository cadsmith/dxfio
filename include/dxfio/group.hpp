#ifndef DXFIO_GROUP_HPP
#define DXFIO_GROUP_HPP

#include "dxfio/api.hpp"
#include "dxfio/handle.hpp"

#include <cstdint>
#include <string>
#include <variant>

namespace DxfIO
{
/// The value of a binary group, 310 to 319 or 1004: the bytes themselves, not the hex an ASCII file spells.
struct DXFIO_API Chunk
{
  /// The bytes, of any value, zero included.
  std::string Bytes{};

  auto operator==(const Chunk &) const -> bool = default;
};

/// The value of a group, of the type its code calls for: text, a whole number, a real, a flag (290 to 299),
/// a handle or bytes. A handle or binary group whose text does not parse stays text.
using Content = std::variant<std::string, std::int64_t, double, bool, Handle, Chunk>;

/// One group of a DXF file: a group code and the value that follows it.
struct DXFIO_API Group
{
  /// The group code, such as 8 for a layer name or 10 for an x coordinate.
  int Code{};
  /// The value, of the type its code calls for.
  Content Value{};

  auto operator==(const Group &) const -> bool = default;
};
} // namespace DxfIO

#endif /* DXFIO_GROUP_HPP */
