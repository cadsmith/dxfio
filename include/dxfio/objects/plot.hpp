#ifndef DXFIO_OBJECTS_PLOT_HPP
#define DXFIO_OBJECTS_PLOT_HPP

#include "dxfio/api.hpp"
#include "dxfio/vector.hpp"

#include <string>

namespace DxfIO::Objects
{
/// The plot flags of a new layout: 16 standard scale, 32 plot styles, 128 lineweights, 512 viewports first.
inline constexpr int Plotted = 688;

/// The plot type that plots the layout itself, its own sheet: what a new layout plots.
inline constexpr int PlotLayout = 5;

/// A standard scale: 1:1.
inline constexpr int Actual = 16;

/// A shade plot resolution: normal.
inline constexpr int Normal = 2;

/// The shade plot resolution of a new layout, in dots per inch.
inline constexpr int Dpi = 300;

/// What a layout or a page setup plots and how: the AcDbPlotSettings part both carry.
struct DXFIO_API Plot
{
  /// The name of the page setup. Group 1.
  std::string Name{};
  /// The printer or plot configuration file. Group 2.
  std::string Device{};
  /// The paper size, as the device names it. Group 4.
  std::string Size{};
  /// The view plotted when Type is 3. Group 6.
  std::string View{};
  /// The unprintable margin on the left of the paper, in millimeters. Group 40.
  double Left{};
  /// The unprintable margin at the bottom, in millimeters. Group 41.
  double Bottom{};
  /// The unprintable margin on the right, in millimeters. Group 42.
  double Right{};
  /// The unprintable margin at the top, in millimeters. Group 43.
  double Top{};
  /// The width of the paper, in millimeters. Group 44.
  double Width{};
  /// The height of the paper, in millimeters. Group 45.
  double Height{};
  /// How far the plot is moved from the paper's origin, in millimeters. Groups 46, 47.
  Vector Origin{};
  /// The lower left corner of the window plotted when Type is 4. Groups 48, 49.
  Vector Lower{};
  /// The upper right corner of the window plotted when Type is 4. Groups 140, 141.
  Vector Upper{};
  /// The paper units of a custom scale. Group 142.
  double Paper{1};
  /// The drawing units of a custom scale. Group 143.
  double Drawing{1};
  /// Bits: 1 viewport borders, 2 plot styles shown, 4 centered, 8 hidden, 16 standard scale, 32 plot styles,
  /// 64 lineweights scaled, 128 lineweights, 512 viewports first, 1024 model space. Group 70.
  int Flags{Plotted};
  /// The units of the paper: 0 inches, 1 millimeters, 2 pixels. Group 72.
  int Units{};
  /// The rotation of the plot: 0 none, 1 a quarter turn counterclockwise, 2 a half turn, 3 a quarter turn
  /// clockwise. Group 73.
  int Rotation{};
  /// What is plotted: 0 the last screen, 1 the extents, 2 the limits, 3 View, 4 the window from Lower to Upper,
  /// 5 the layout. Group 74.
  int Type{PlotLayout};
  /// The plot style table, such as "acad.ctb". Group 7.
  std::string Style{};
  /// The standard scale: 0 scaled to fit, 16 1:1, others for the scales AutoCAD lists. Group 75.
  int Standard{Actual};
  /// The standard scale as a number. Group 147.
  double Factor{1};
  /// How 3D views are shaded: 0 as displayed, 1 wireframe, 2 hidden, 3 rendered. Group 76, from R2004.
  int Shading{};
  /// The resolution of shaded plots: 0 draft, 1 preview, 2 normal, 3 presentation, 4 maximum, 5 custom.
  /// Group 77, from R2004.
  int Resolution{Normal};
  /// The dots per inch of shaded plots when Resolution is custom. Group 78, from R2004.
  int Dots{Dpi};
  /// The origin of the paper image. Groups 148, 149.
  Vector Image{};

  auto operator==(const Plot &) const -> bool = default;
};
} // namespace DxfIO::Objects

#endif /* DXFIO_OBJECTS_PLOT_HPP */
