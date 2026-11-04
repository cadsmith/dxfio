#ifndef DXFIO_ENTITIES_CIRCLE_HPP
#define DXFIO_ENTITIES_CIRCLE_HPP

#include "dxfio/api.hpp"
#include "dxfio/entities/record.hpp"
#include "dxfio/vector.hpp"

namespace DxfIO::Entities
{
/// A CIRCLE: a full circle in the plane of its normal.
struct DXFIO_API Circle
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

  auto operator==(const Circle &) const -> bool = default;
};
} // namespace DxfIO::Entities

#endif /* DXFIO_ENTITIES_CIRCLE_HPP */
