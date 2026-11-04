#ifndef DXFIO_ENTITIES_ARC_HPP
#define DXFIO_ENTITIES_ARC_HPP

#include "dxfio/api.hpp"
#include "dxfio/entities/record.hpp"
#include "dxfio/vector.hpp"

namespace DxfIO::Entities
{
/// An ARC: part of a circle, drawn counterclockwise from its start angle to its end angle.
struct DXFIO_API Arc
{
  /// The layer, linetype, color and the rest every entity carries.
  Entities::Record Record{};
  /// How far the entity is extruded along its normal. Group 39.
  double Thickness{};
  /// The center, in the entity's own coordinate system. Groups 10, 20, 30.
  Vector Center{};
  /// The radius, in drawing units. Group 40.
  double Radius{};
  /// The normal of the plane the entity is drawn in. Groups 210, 220, 230.
  Vector Extrusion{Up};
  /// The start angle, in degrees from the X axis of the entity's own coordinate system. Group 50.
  double Start{};
  /// The end angle, in degrees from the same axis. Group 51.
  double End{};

  auto operator==(const Arc &) const -> bool = default;
};
} // namespace DxfIO::Entities

#endif /* DXFIO_ENTITIES_ARC_HPP */
