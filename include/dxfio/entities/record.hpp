#ifndef DXFIO_ENTITIES_RECORD_HPP
#define DXFIO_ENTITIES_RECORD_HPP

#include "dxfio/api.hpp"
#include "dxfio/base.hpp"
#include "dxfio/colors.hpp"
#include "dxfio/handle.hpp"
#include "dxfio/lineweights.hpp"
#include "dxfio/symbol.hpp"

#include <optional>
#include <string>

namespace DxfIO::Entities
{
/// What every entity carries in its AcDbEntity subclass: its layer, linetype, color and the rest.
struct DXFIO_API Record
{
  /// The handle, owner, reactors and extended data.
  DxfIO::Base Base{};
  /// Whether the entity lies in paper space rather than model space. Group 67.
  bool Paper{};
  /// The name of the layer the entity is on. Group 8.
  Symbol Layer{"0"};
  /// The name of the linetype, or ByLayer or ByBlock. Group 6.
  Symbol Linetype{"ByLayer"};
  /// The material, or none to take the layer's. Group 347, from R2007.
  Handle Material{};
  /// The AutoCAD Color Index: 1 to 255, 0 for ByBlock, 256 for ByLayer. Group 62.
  int Color{Colors::Bylayer};
  /// The true color as 0xRRGGBB, when the entity has one. Group 420, from R2004.
  std::optional<int> Rgb;
  /// The color book and the color's name in it, joined by a $, when the color comes from one. Group 430, from R2004.
  std::optional<std::string> Book;
  /// The transparency, when the entity has its own rather than its layer's. Group 440, from R2004.
  std::optional<int> Transparency;
  /// The scale of the linetype for this entity. Group 48, from R13.
  double Scale{1};
  /// Whether the entity is invisible. Group 60, from R13.
  bool Hidden{};
  /// The lineweight in hundredths of a millimeter, or -1 ByLayer, -2 ByBlock, -3 the default. Group 370, from R2000.
  int Lineweight{Lineweights::Bylayer};
  /// The plot style, when the drawing names its plot styles. Group 390, from R2000.
  Handle Style{};
  /// How the entity takes part in shadows: 0 casts and receives, 1 casts, 2 receives, 3 neither. Group 284, from R2007.
  int Shadow{};

  auto operator==(const Record &) const -> bool = default;
};
} // namespace DxfIO::Entities

#endif /* DXFIO_ENTITIES_RECORD_HPP */
