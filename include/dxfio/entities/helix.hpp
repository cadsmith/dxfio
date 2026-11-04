#ifndef DXFIO_ENTITIES_HELIX_HPP
#define DXFIO_ENTITIES_HELIX_HPP

#include "dxfio/api.hpp"
#include "dxfio/boxed.hpp"
#include "dxfio/entities/record.hpp"
#include "dxfio/entities/spline.hpp"
#include "dxfio/vector.hpp"

#include <string>

namespace DxfIO::Entities
{
/// The AcDbHelix subclass of a HELIX: the axis, radius and turns the coil follows.
struct DXFIO_API Coil
{
  /// The major version of the helix's layout. Group 90.
  int Major{};
  /// The minor version of the helix's layout. Group 91.
  int Minor{};
  /// The base point of the axis, in world coordinates. Groups 10, 20, 30.
  Vector Base{};
  /// The point the helix starts at, in world coordinates. Groups 11, 21, 31.
  Vector Start{};
  /// The direction of the axis. Groups 12, 22, 32.
  Vector Axis{Up};
  /// The radius, in drawing units. Group 40.
  double Radius{1};
  /// The number of turns. Group 41.
  double Turns{1};
  /// The height of one turn. Group 42.
  double Rise{1};
  /// Whether the helix turns right-handed. Group 290.
  bool Right{true};
  /// What is kept when the helix is edited: 0 the height of a turn, 1 the turns, 2 the height. Group 280.
  int Constraint{};

  auto operator==(const Coil &) const -> bool = default;
};

/// A HELIX: a spiral in 3D, kept both as a spline and as its axis and turns.
struct DXFIO_API Helix
{
  /// The layer, linetype, color and the rest every entity carries.
  Entities::Record Record{};
  /// The proxy graphics, for programs that cannot draw the entity. Group 92, or 160 from R2010, then 310.
  std::string Picture;
  /// The spline that draws the helix, from the AcDbSpline subclass.
  Boxed<Entities::Curve> Curve;
  /// The axis, radius and turns, from the AcDbHelix subclass.
  Entities::Coil Coil{};

  auto operator==(const Helix &) const -> bool = default;
};
} // namespace DxfIO::Entities

#endif /* DXFIO_ENTITIES_HELIX_HPP */
