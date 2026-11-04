#ifndef DXFIO_ENTITIES_ELLIPSE_HPP
#define DXFIO_ENTITIES_ELLIPSE_HPP

#include "dxfio/api.hpp"
#include "dxfio/entities/record.hpp"
#include "dxfio/vector.hpp"

#include <numbers>

namespace DxfIO::Entities
{
/// A full turn, in radians: the end parameter of a closed ellipse.
inline constexpr double Turn = 2 * std::numbers::pi;

/// An ELLIPSE: a full or partial ellipse, given by its center, its major axis and the ratio of its minor axis.
struct DXFIO_API Ellipse
{
  /// The layer, linetype, color and the rest every entity carries.
  Entities::Record Record{};
  /// The center, in world coordinates. Groups 10, 20, 30.
  Vector Center{};
  /// The end of the major axis, relative to the center. Groups 11, 21, 31.
  Vector Axis{Right};
  /// The normal of the plane the entity is drawn in. Groups 210, 220, 230.
  Vector Extrusion{Up};
  /// The length of the minor axis over that of the major axis. Group 40.
  double Ratio{1};
  /// The parameter the curve starts at, in radians, 0 at the end of the major axis. Group 41.
  double Start{};
  /// The parameter the curve ends at, in radians; a full turn past the start closes the ellipse. Group 42.
  double End{Turn};

  auto operator==(const Ellipse &) const -> bool = default;
};
} // namespace DxfIO::Entities

#endif /* DXFIO_ENTITIES_ELLIPSE_HPP */
