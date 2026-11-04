#ifndef DXFIO_TABLES_STYLE_HPP
#define DXFIO_TABLES_STYLE_HPP

#include "dxfio/api.hpp"
#include "dxfio/tables/record.hpp"

#include <string>

namespace DxfIO::Tables
{
/// The last height a new text style was used at.
inline constexpr double Last = 2.5;

/// A STYLE table record: the font and proportions text is drawn with. Its flags are 1 for a shape file
/// rather than a text style and 4 for vertical text.
struct DXFIO_API Style
{
  /// The name, flags and handles every table record carries.
  Tables::Record Record{};
  /// The height every text of the style takes; 0 when each text gives its own. Group 40.
  double Height{};
  /// The width of characters over their height. Group 41.
  double Width{1};
  /// The slant of characters, in degrees. Group 50.
  double Oblique{};
  /// Bits: 2 for text drawn backward, 4 for text drawn upside down. Group 71.
  int Generation{};
  /// The height the style was last used at. Group 42.
  double Previous{Last};
  /// The font file, such as "txt" or "arial.ttf". Group 3.
  std::string Font{"txt"};
  /// The big font file for Asian characters; empty for none. Group 4.
  std::string Bigfont{};

  auto operator==(const Style &) const -> bool = default;
};
} // namespace DxfIO::Tables

#endif /* DXFIO_TABLES_STYLE_HPP */
