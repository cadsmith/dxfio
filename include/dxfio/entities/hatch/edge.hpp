#ifndef DXFIO_ENTITIES_HATCH_EDGE_HPP
#define DXFIO_ENTITIES_HATCH_EDGE_HPP

#include "dxfio/api.hpp"
#include "dxfio/boxed.hpp"
#include "dxfio/vector.hpp"

#include <optional>
#include <variant>
#include <vector>

namespace DxfIO::Entities::Edges
{
/// A vertex of a polyline boundary, or a control point of a spline edge, with its bulge or weight.
struct DXFIO_API Node
{
  /// The location, in the hatch's own coordinate system. Groups 10, 20.
  Vector Position{};
  /// The bulge of the segment to the next vertex, or the weight of the control point, when it has one. Group 42.
  std::optional<double> Weight{};

  auto operator==(const Node &) const -> bool = default;
};

/// A point of a hatch: a seed point, or a fit point of a spline edge.
struct DXFIO_API Place
{
  /// The location, in the hatch's own coordinate system. Groups 10, 20 for a seed point, 11, 21 for a fit point.
  Vector Position{};

  auto operator==(const Place &) const -> bool = default;
};

/// A straight edge of a hatch boundary.
struct DXFIO_API Line
{
  /// The start point, in the hatch's own coordinate system. Groups 10, 20.
  Vector Start{};
  /// The end point, in the hatch's own coordinate system. Groups 11, 21.
  Vector End{};

  auto operator==(const Line &) const -> bool = default;
};

/// A circular arc edge of a hatch boundary.
struct DXFIO_API Arc
{
  /// The center, in the hatch's own coordinate system. Groups 10, 20.
  Vector Center{};
  /// The radius, in drawing units. Group 40.
  double Radius{};
  /// The start angle, in degrees. Group 50.
  double Start{};
  /// The end angle, in degrees. Group 51.
  double End{};
  /// 1 when the arc runs counterclockwise. Group 73.
  int Counter{};

  auto operator==(const Arc &) const -> bool = default;
};

/// An elliptic arc edge of a hatch boundary.
struct DXFIO_API Ellipse
{
  /// The center, in the hatch's own coordinate system. Groups 10, 20.
  Vector Center{};
  /// The end of the major axis, relative to the center. Groups 11, 21.
  Vector Major{};
  /// The length of the minor axis over that of the major axis. Group 40.
  double Ratio{};
  /// The start angle, in degrees. Group 50.
  double Start{};
  /// The end angle, in degrees. Group 51.
  double End{};
  /// 1 when the arc runs counterclockwise. Group 73.
  int Counter{};

  auto operator==(const Ellipse &) const -> bool = default;
};

/// A spline edge of a hatch boundary.
struct DXFIO_API Spline
{
  /// The degree of the spline, 3 for a new one. Group 94.
  int Degree{3};
  /// 1 when the spline is rational. Group 73.
  int Rational{};
  /// 1 when the spline is periodic. Group 74.
  int Periodic{};
  /// The knot values; group 95 counts them. Group 40.
  std::vector<double> Knots{};
  /// The control points, with their weights when the spline is rational; group 96 counts them.
  std::vector<Node> Poles{};
  /// The points the spline passes through; group 97 counts them. From R2010.
  std::vector<Place> Fits{};
  /// The tangent at the start, when the spline has fit points. Groups 12, 22, from R2010.
  Vector Start{};
  /// The tangent at the end, when the spline has fit points. Groups 13, 23, from R2010.
  Vector End{};

  auto operator==(const Spline &) const -> bool = default;
};

/// An edge of a hatch boundary, its kind given by group 72: 1 line, 2 circular arc, 3 elliptic arc, 4 spline.
/// A spline is kept on the heap, so an edge stays the size of an elliptic arc.
using Edge = std::variant<Line, Arc, Ellipse, Boxed<Spline>>;
} // namespace DxfIO::Entities::Edges

#endif /* DXFIO_ENTITIES_HATCH_EDGE_HPP */
