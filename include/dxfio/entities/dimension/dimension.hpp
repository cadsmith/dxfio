#ifndef DXFIO_ENTITIES_DIMENSION_DIMENSION_HPP
#define DXFIO_ENTITIES_DIMENSION_DIMENSION_HPP

#include "dxfio/api.hpp"
#include "dxfio/boxed.hpp"
#include "dxfio/entities/dimension/measure.hpp"
#include "dxfio/entities/record.hpp"
#include "dxfio/vector.hpp"

#include <string>
#include <variant>

namespace DxfIO::Entities
{
namespace Forms
{
/// A rotated linear dimension: the distance between two points along a direction.
struct DXFIO_API Linear
{
  /// The first point measured, in world coordinates. Groups 13, 23, 33.
  Vector Start{};
  /// The second point measured, in world coordinates. Groups 14, 24, 34.
  Vector End{};
  /// The direction the distance is measured along, in degrees. Group 50.
  double Angle{};

  auto operator==(const Linear &) const -> bool = default;
};

/// An aligned dimension: the distance between two points, along the line through them.
struct DXFIO_API Aligned
{
  /// The first point measured, in world coordinates. Groups 13, 23, 33.
  Vector Start{};
  /// The second point measured, in world coordinates. Groups 14, 24, 34.
  Vector End{};
  /// The rotation of the dimension line, in degrees. Group 50.
  double Angle{};

  auto operator==(const Aligned &) const -> bool = default;
};

/// An angular dimension between two lines.
struct DXFIO_API Angular
{
  /// The start of the first line, in world coordinates. Groups 13, 23, 33.
  Vector Start{};
  /// The end of the first line, in world coordinates. Groups 14, 24, 34.
  Vector End{};
  /// The start of the second line, which the definition point ends, in world coordinates. Groups 15, 25, 35.
  Vector From{};
  /// A point on the dimension arc, in the entity's own coordinate system. Groups 16, 26, 36.
  Vector To{};

  auto operator==(const Angular &) const -> bool = default;
};

/// A diameter dimension of a circle or an arc.
struct DXFIO_API Diameter
{
  /// The point on the curve across from the definition point, in world coordinates. Groups 15, 25, 35.
  Vector Chord{};
  /// The length of the leader. Group 40.
  double Leader{};

  auto operator==(const Diameter &) const -> bool = default;
};

/// A radius dimension of a circle or an arc.
struct DXFIO_API Radial
{
  /// The point on the curve, in world coordinates; the definition point is the center. Groups 15, 25, 35.
  Vector Chord{};
  /// The length of the leader. Group 40.
  double Leader{};

  auto operator==(const Radial &) const -> bool = default;
};

/// An angular dimension by three points: the vertex of the angle and a point on each side.
struct DXFIO_API Points
{
  /// A point on the first side, in world coordinates. Groups 13, 23, 33.
  Vector Start{};
  /// A point on the second side, in world coordinates. Groups 14, 24, 34.
  Vector End{};
  /// The vertex of the angle, in world coordinates. Groups 15, 25, 35.
  Vector Vertex{};

  auto operator==(const Points &) const -> bool = default;
};

/// An ordinate dimension: the X or Y distance of a feature from the UCS origin.
struct DXFIO_API Ordinate
{
  /// The feature measured, in world coordinates. Groups 13, 23, 33.
  Vector Feature{};
  /// The end of the leader, in world coordinates. Groups 14, 24, 34.
  Vector Leader{};

  auto operator==(const Ordinate &) const -> bool = default;
};

/// The kind of a DIMENSION, as the low three bits of group 70 give it, with the points that kind adds. It is what
/// those bits are written from.
using Form = std::variant<Linear, Aligned, Angular, Diameter, Radial, Points, Ordinate>;
} // namespace Forms

/// What a DIMENSION draws: what every dimension shares, and the points of its kind.
struct DXFIO_API Figure
{
  /// The block, the text and the plane, from the AcDbDimension subclass.
  Entities::Measure Measure{};
  /// The kind of dimension and its points, from the subclass of that kind.
  Forms::Form Form;

  auto operator==(const Figure &) const -> bool = default;
};

/// A DIMENSION: a linear, aligned, angular, diameter, radius or ordinate dimension.
struct DXFIO_API Dimension
{
  /// The layer, linetype, color and the rest every entity carries.
  Entities::Record Record{};
  /// What the dimension measures and how it is drawn.
  Boxed<Entities::Figure> Figure;

  auto operator==(const Dimension &) const -> bool = default;
};

/// What an ARC_DIMENSION draws: what every dimension shares, and the arc it measures.
struct DXFIO_API Bend
{
  /// The block, the text and the plane, from the AcDbDimension subclass.
  Entities::Measure Measure{};
  /// Where the arc starts, in world coordinates. Groups 13, 23, 33.
  Vector Start{};
  /// Where the arc ends, in world coordinates. Groups 14, 24, 34.
  Vector End{};
  /// The center of the arc, in world coordinates. Groups 15, 25, 35.
  Vector Center{};
  /// 1 when the dimension measures part of the arc. Group 70.
  int Partial{};
  /// The angle the measured part starts at. Group 40.
  double From{};
  /// The angle the measured part ends at. Group 41.
  double To{};
  /// 1 when the dimension has a leader. Group 71.
  int Led{};
  /// The first point of the leader, in world coordinates. Groups 16, 26, 36.
  Vector Near{};
  /// The second point of the leader, in world coordinates. Groups 17, 27, 37.
  Vector Far{};

  auto operator==(const Bend &) const -> bool = default;
};

/// An ARC_DIMENSION: the length along an arc or an arc segment of a polyline.
struct DXFIO_API ArcDimension
{
  /// The layer, linetype, color and the rest every entity carries.
  Entities::Record Record{};
  /// The proxy graphics, for programs that cannot draw the entity. Group 92, or 160 from R2010, then 310.
  std::string Picture;
  /// The arc measured and how the dimension is drawn, from the AcDbArcDimension subclass.
  Boxed<Entities::Bend> Bend;

  auto operator==(const ArcDimension &) const -> bool = default;
};

/// What a LARGE_RADIAL_DIMENSION draws: what every dimension shares, and the jog it is drawn with.
struct DXFIO_API Jog
{
  /// The block, the text and the plane, from the AcDbDimension subclass.
  Entities::Measure Measure{};
  /// The point on the curve, in world coordinates. Groups 13, 23, 33.
  Vector Chord{};
  /// The center the dimension is drawn from, in place of the curve's own, in world coordinates. Groups 14, 24, 34.
  Vector Center{};
  /// The point of the jog, in world coordinates. Groups 15, 25, 35.
  Vector Turn{};
  /// The angle of the jog. Group 40.
  double Angle{};

  auto operator==(const Jog &) const -> bool = default;
};

/// A LARGE_RADIAL_DIMENSION: a jogged radius dimension, for an arc whose center lies far off.
struct DXFIO_API LargeRadialDimension
{
  /// The layer, linetype, color and the rest every entity carries.
  Entities::Record Record{};
  /// The proxy graphics, for programs that cannot draw the entity. Group 92, or 160 from R2010, then 310.
  std::string Picture;
  /// The curve measured and the jog, from the AcDbRadialDimensionLarge subclass.
  Boxed<Entities::Jog> Jog;

  auto operator==(const LargeRadialDimension &) const -> bool = default;
};
} // namespace DxfIO::Entities

#endif /* DXFIO_ENTITIES_DIMENSION_DIMENSION_HPP */
