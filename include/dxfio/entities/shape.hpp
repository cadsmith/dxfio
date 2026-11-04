#ifndef DXFIO_ENTITIES_SHAPE_HPP
#define DXFIO_ENTITIES_SHAPE_HPP

#include "dxfio/api.hpp"
#include "dxfio/entities/record.hpp"
#include "dxfio/symbol.hpp"
#include "dxfio/vector.hpp"

namespace DxfIO::Entities
{
/// A SHAPE: a symbol from a compiled shape file, which the drawing loads through a text style.
struct DXFIO_API Shape
{
  /// The layer, linetype, color and the rest every entity carries.
  Entities::Record Record{};
  /// How far the entity is extruded along its normal. Group 39.
  double Thickness{};
  /// The insertion point, in the entity's own coordinate system. Groups 10, 20, 30.
  Vector Position{};
  /// The size, in drawing units. Group 40.
  double Size{};
  /// The name of the shape in its file. Group 2.
  Symbol Name{""};
  /// The rotation, in degrees. Group 50.
  double Rotation{};
  /// The relative width, 1 for the shape's own. Group 41.
  double Width{1};
  /// The slant, in degrees from the vertical. Group 51.
  double Oblique{};
  /// The normal of the plane the entity is drawn in. Groups 210, 220, 230.
  Vector Extrusion{Up};

  auto operator==(const Shape &) const -> bool = default;
};
} // namespace DxfIO::Entities

#endif /* DXFIO_ENTITIES_SHAPE_HPP */
