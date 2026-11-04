#ifndef DXFIO_ENTITIES_POINT_HPP
#define DXFIO_ENTITIES_POINT_HPP

#include "dxfio/api.hpp"
#include "dxfio/entities/record.hpp"
#include "dxfio/vector.hpp"

namespace DxfIO::Entities
{
/// A POINT: a single point, drawn in the style the drawing's $PDMODE sets.
struct DXFIO_API Point
{
  /// The layer, linetype, color and the rest every entity carries.
  Entities::Record Record{};
  /// The location, in world coordinates. Groups 10, 20, 30.
  Vector Position{};
  /// How far the entity is extruded along its normal. Group 39.
  double Thickness{};
  /// The normal of the plane the entity is drawn in. Groups 210, 220, 230.
  Vector Extrusion{Up};
  /// The angle of the X axis of the UCS the point was drawn in, which turns its symbol when $PDMODE is set. Group 50.
  double Angle{};

  auto operator==(const Point &) const -> bool = default;
};
} // namespace DxfIO::Entities

#endif /* DXFIO_ENTITIES_POINT_HPP */
