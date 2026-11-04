#ifndef DXFIO_OBJECTS_LAYOUT_HPP
#define DXFIO_OBJECTS_LAYOUT_HPP

#include "dxfio/api.hpp"
#include "dxfio/base.hpp"
#include "dxfio/boxed.hpp"
#include "dxfio/handle.hpp"
#include "dxfio/objects/plot.hpp"
#include "dxfio/tables/axes.hpp"
#include "dxfio/vector.hpp"

#include <string>

namespace DxfIO::Objects
{
/// The upper right limits of a new layout: 12 by 9.
inline constexpr Vector Limits{.X = 12, .Y = 9, .Z = 0};

/// A LAYOUT: the model space tab or a paper space sheet, with how it plots.
struct DXFIO_API Layout
{
  /// The handle, owner, reactors and extended data.
  DxfIO::Base Base{};
  /// How the layout plots, as a PLOTSETTINGS gives it; kept on the heap, so every object stays small.
  Boxed<Objects::Plot> Plot{};
  /// The name on the layout's tab, such as "Model" or "Layout1". Group 1.
  std::string Name{};
  /// Bits: 1 to scale linetypes by the viewport, as $PSLTSCALE; 2 to keep drawing within the limits, as
  /// $LIMCHECK. Group 70.
  int Flags{1};
  /// The place of the tab among the layouts; 0 for model space. Group 71.
  int Order{};
  /// The lower left limits. Groups 10, 20.
  Vector Limmin{};
  /// The upper right limits. Groups 11, 21.
  Vector Limmax{Limits};
  /// The base point blocks are inserted by. Groups 12, 22, 32.
  Vector Insbase{};
  /// The lower left corner of what is drawn. Groups 14, 24, 34.
  Vector Extmin{};
  /// The upper right corner of what is drawn. Groups 15, 25, 35.
  Vector Extmax{};
  /// The UCS of the layout.
  Tables::Axes Axes{};
  /// The BLOCK_RECORD of the layout's space. Group 330.
  Handle Block{};
  /// The viewport last active in the layout. Group 331.
  Handle Viewport{};

  auto operator==(const Layout &) const -> bool = default;
};
} // namespace DxfIO::Objects

#endif /* DXFIO_OBJECTS_LAYOUT_HPP */
