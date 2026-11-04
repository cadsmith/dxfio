#ifndef DXFIO_VERSION_HPP
#define DXFIO_VERSION_HPP

#include "dxfio/api.hpp"

#include <cstdint>
#include <optional>
#include <string_view>

namespace DxfIO
{
/// A release of DXF, as $ACADVER names it, oldest first so that releases compare by age.
enum class Version : std::uint8_t
{
  /// AutoCAD Release 10, AC1006.
  R10,
  /// AutoCAD Release 11 and 12, AC1009.
  R12,
  /// AutoCAD Release 13, AC1012: the first with subclass markers, classes and objects.
  R13,
  /// AutoCAD Release 14, AC1014.
  R14,
  /// AutoCAD 2000 to 2002, AC1015.
  R2000,
  /// AutoCAD 2004 to 2006, AC1018.
  R2004,
  /// AutoCAD 2007 to 2009, AC1021: the first to write its text in UTF-8 rather than a code page.
  R2007,
  /// AutoCAD 2010 to 2012, AC1024.
  R2010,
  /// AutoCAD 2013 to 2017, AC1027.
  R2013,
  /// AutoCAD 2018 and later, AC1032.
  R2018,
};

/// The $ACADVER code of a release, such as "AC1015" for R2000.
[[nodiscard]] DXFIO_API auto Acadver(Version release) -> std::string_view;

/// The short name of a release, such as "R2000".
[[nodiscard]] DXFIO_API auto Name(Version release) -> std::string_view;

/// The release an $ACADVER code such as "AC1015" stands for: the newest at or below it, or R10 below AC1006.
/// Empty unless the code, blanks around it trimmed, is "AC" in either case and four digits.
[[nodiscard]] DXFIO_API auto Release(std::string_view code) -> std::optional<Version>;
} // namespace DxfIO

#endif /* DXFIO_VERSION_HPP */
