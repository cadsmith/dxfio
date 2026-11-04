#ifndef DXFIO_OBJECTS_PLOTSETTINGS_HPP
#define DXFIO_OBJECTS_PLOTSETTINGS_HPP

#include "dxfio/api.hpp"
#include "dxfio/base.hpp"
#include "dxfio/boxed.hpp"
#include "dxfio/objects/plot.hpp"

namespace DxfIO::Objects
{
/// A PLOTSETTINGS: a named page setup a layout can take its plot settings from.
struct DXFIO_API PlotSettings
{
  /// The handle, owner, reactors and extended data.
  DxfIO::Base Base{};
  /// What the page setup plots and how; kept on the heap, so every object stays small.
  Boxed<Objects::Plot> Plot{};

  auto operator==(const PlotSettings &) const -> bool = default;
};
} // namespace DxfIO::Objects

#endif /* DXFIO_OBJECTS_PLOTSETTINGS_HPP */
