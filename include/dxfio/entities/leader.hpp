#ifndef DXFIO_ENTITIES_LEADER_HPP
#define DXFIO_ENTITIES_LEADER_HPP

#include "dxfio/api.hpp"
#include "dxfio/boxed.hpp"
#include "dxfio/entities/record.hpp"
#include "dxfio/handle.hpp"
#include "dxfio/symbol.hpp"
#include "dxfio/vector.hpp"

#include <optional>
#include <string>
#include <vector>

namespace DxfIO::Entities
{
/// The annotation type of a leader made without an annotation, which group 73 takes when it is left out.
inline constexpr int Unannotated = 3;

/// The AcDbLeader subclass of a LEADER: its vertices, its arrowhead and the annotation it points from.
struct DXFIO_API Lead
{
  /// The name of the dimension style. Group 3.
  Symbol Style{};
  /// 1 when the leader has an arrowhead, 0 when it has none. Group 71.
  int Arrow{1};
  /// The path: 0 straight segments, 1 a spline. Group 72.
  int Path{};
  /// What the leader was made with: 0 an MTEXT, 1 a TOLERANCE, 2 a block reference, 3 no annotation. Group 73.
  int Annotation{Unannotated};
  /// 1 when the hookline runs along the horizontal direction, 0 when it runs against it. Group 74.
  int Direction{1};
  /// 1 when the leader ends in a hookline, 0 when it does not. Group 75.
  int Hook{};
  /// The height of the text annotation. Group 40.
  std::optional<double> Height{};
  /// The width of the text annotation. Group 41.
  std::optional<double> Width{};
  /// The vertices, in world coordinates; group 76 counts them. Groups 10, 20, 30.
  std::vector<Vector> Points{};
  /// The color of the leader when its dimension style's DIMCLRD is ByBlock. Group 77.
  std::optional<int> Color{};
  /// The annotation: an MTEXT, a TOLERANCE or an INSERT. Group 340.
  Handle Note{};
  /// The normal of the plane the entity is drawn in. Groups 210, 220, 230.
  Vector Normal{Up};
  /// The horizontal direction of the leader. Groups 211, 221, 231.
  Vector Horizontal{.X = 1};
  /// The offset of the last vertex from the insertion point of the block reference it annotates.
  /// Groups 212, 222, 232.
  Vector Block{};
  /// The offset of the last vertex from where the annotation is placed. Groups 213, 223, 233.
  Vector Offset{};

  auto operator==(const Lead &) const -> bool = default;
};

/// A LEADER: a line with an arrowhead, from a feature to its annotation.
struct DXFIO_API Leader
{
  /// The layer, linetype, color and the rest every entity carries.
  Entities::Record Record{};
  /// The vertices, arrowhead and annotation, from the AcDbLeader subclass.
  Boxed<Entities::Lead> Lead{};

  auto operator==(const Leader &) const -> bool = default;
};
} // namespace DxfIO::Entities

#endif /* DXFIO_ENTITIES_LEADER_HPP */
