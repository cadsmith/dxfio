#ifndef DXFIO_TABLES_LAYER_HPP
#define DXFIO_TABLES_LAYER_HPP

#include "dxfio/api.hpp"
#include "dxfio/colors.hpp"
#include "dxfio/handle.hpp"
#include "dxfio/lineweights.hpp"
#include "dxfio/symbol.hpp"
#include "dxfio/tables/record.hpp"

#include <optional>
#include <string>

namespace DxfIO::Tables
{
/// The color of a new layer: white, index 7.
inline constexpr int White = Colors::White;

/// A LAYER table record: how the entities on a layer are drawn. Its flags are 1 frozen, 2 frozen in new
/// viewports and 4 locked.
struct DXFIO_API Layer
{
  /// The name, flags and handles every table record carries.
  Tables::Record Record{};
  /// The AutoCAD Color Index of the layer, 1 to 255; negative when the layer is off. Group 62.
  int Color{White};
  /// The name of the layer's linetype. Group 6.
  Symbol Linetype{"Continuous"};
  /// Whether the layer is plotted. Group 290, from R2000.
  bool Plot{true};
  /// The lineweight in hundredths of a millimeter, or -3 for the default. Group 370, from R2000.
  int Lineweight{Lineweights::Standard};
  /// The plot style, a placeholder in the plot style name dictionary. Group 390, from R2000.
  Handle Style{};
  /// The material of the layer's entities. Group 347, from R2007.
  Handle Material{};
  /// The true color as 0xRRGGBB, when the layer has one. Group 420, from R2004.
  std::optional<int> Rgb;
  /// The color book and the color's name in it, joined by a $, when the color comes from one. Group 430, from R2004.
  std::optional<std::string> Book;
  /// A handle AutoCAD writes with no published meaning, usually none. Group 348, from R2013.
  Handle Spare{};

  auto operator==(const Layer &) const -> bool = default;
};
} // namespace DxfIO::Tables

#endif /* DXFIO_TABLES_LAYER_HPP */
