#ifndef DXFIO_ENTITIES_SPLINE_HPP
#define DXFIO_ENTITIES_SPLINE_HPP

#include "dxfio/api.hpp"
#include "dxfio/boxed.hpp"
#include "dxfio/entities/record.hpp"
#include "dxfio/vector.hpp"

#include <optional>
#include <vector>

namespace DxfIO::Entities
{
/// The knot tolerance a new spline takes.
inline constexpr double Knotting = 1e-9;

/// The control point tolerance a new spline takes.
inline constexpr double Framing = 1e-10;

/// The AcDbSpline subclass a SPLINE and a HELIX share: a NURBS curve, by its knots and control points
/// or by the points it fits.
struct DXFIO_API Curve
{
  /// The normal of the plane of a planar spline; none for one that is not planar. Groups 210, 220, 230.
  std::optional<Vector> Normal{};
  /// 1 closed, 2 periodic, 4 rational, 8 planar, 16 linear, which is planar too. Group 70.
  int Flags{};
  /// The degree of the curve, 3 for a new one. Group 71.
  int Degree{3};
  /// The knot tolerance. Group 42.
  double Knotted{Knotting};
  /// The control point tolerance. Group 43.
  double Framed{Framing};
  /// The fit tolerance, when the spline is given by fit points. Group 44.
  std::optional<double> Fitted{};
  /// The tangent at the start, when one is given. Groups 12, 22, 32.
  std::optional<Vector> Start{};
  /// The tangent at the end, when one is given. Groups 13, 23, 33.
  std::optional<Vector> End{};
  /// The knot values; group 72 counts them. Group 40.
  std::vector<double> Knots{};
  /// The control points, in world coordinates; group 73 counts them. Groups 10, 20, 30.
  std::vector<Vector> Poles{};
  /// The weight of each control point, when the spline is rational. Group 41.
  std::vector<double> Weights{};
  /// The points the curve passes through, in world coordinates; group 74 counts them. Groups 11, 21, 31.
  std::vector<Vector> Fits{};

  auto operator==(const Curve &) const -> bool = default;
};

/// A SPLINE: a smooth curve through or near a set of points.
struct DXFIO_API Spline
{
  /// The layer, linetype, color and the rest every entity carries.
  Entities::Record Record{};
  /// The knots, control points and fit points, from the AcDbSpline subclass.
  Boxed<Entities::Curve> Curve{};

  auto operator==(const Spline &) const -> bool = default;
};
} // namespace DxfIO::Entities

#endif /* DXFIO_ENTITIES_SPLINE_HPP */
