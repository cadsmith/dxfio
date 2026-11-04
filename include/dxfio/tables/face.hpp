#ifndef DXFIO_TABLES_FACE_HPP
#define DXFIO_TABLES_FACE_HPP

#include "dxfio/api.hpp"
#include "dxfio/tables/style.hpp"

#include <cstdint>
#include <optional>
#include <string>

namespace DxfIO::Tables
{
/// The TrueType face a text style uses, as AutoCAD keeps it in the style's ACAD extended data.
struct DXFIO_API Face
{
  /// The font family, such as "Arial". Extended data group 1000.
  std::string Family{};
  /// The face's bold, italic, character set and pitch, packed as AutoCAD writes them. Extended data group 1071.
  std::int64_t Flags{};

  auto operator==(const Face &) const -> bool = default;
};

/// The TrueType face a text style names in its extended data, or none when it uses a font file alone.
[[nodiscard]] DXFIO_API auto Named(const Style &one) -> std::optional<Face>;
} // namespace DxfIO::Tables

#endif /* DXFIO_TABLES_FACE_HPP */
