#ifndef DXFIO_ENTITIES_POLYLINE_HPP
#define DXFIO_ENTITIES_POLYLINE_HPP

#include "dxfio/api.hpp"
#include "dxfio/entities/record.hpp"
#include "dxfio/entities/sequence.hpp"
#include "dxfio/entities/vertex.hpp"
#include "dxfio/vector.hpp"

namespace DxfIO::Entities
{
/// A POLYLINE: a 2D or 3D polyline, a polygon mesh or a polyface mesh, its vertices following it in the file.
struct DXFIO_API Polyline
{
  /// The layer, linetype, color and the rest every entity carries.
  Entities::Record Record{};
  /// Whether VERTEX entities follow it in the file. Group 66.
  bool Follows{true};
  /// A point whose Z is the polyline's elevation; its X and Y are 0. Groups 10, 20, 30.
  Vector Position{};
  /// How far the entity is extruded along its normal. Group 39.
  double Thickness{};
  /// 1 closed, 2 curve-fit, 4 spline-fit, 8 3D polyline, 16 polygon mesh, 32 mesh closed along N,
  /// 64 polyface mesh, 128 linetype drawn continuously around the vertices. Group 70.
  int Flags{};
  /// The width segments start with when their vertex gives none. Group 40.
  double Start{};
  /// The width segments end with when their vertex gives none. Group 41.
  double End{};
  /// The vertex count along M of a polygon mesh, or the vertex count of a polyface mesh. Group 71.
  int Rows{};
  /// The vertex count along N of a polygon mesh, or the face count of a polyface mesh. Group 72.
  int Columns{};
  /// The density along M of a smoothed polygon mesh. Group 73.
  int Density{};
  /// The density along N of a smoothed polygon mesh. Group 74.
  int Depth{};
  /// How the surface is smoothed: 0 not at all, 5 quadratic B-spline, 6 cubic B-spline, 8 Bezier. Group 75.
  int Curve{};
  /// The normal of the plane the entity is drawn in. Groups 210, 220, 230.
  Vector Extrusion{Up};
  /// The vertices that follow it in the file, closed by a SEQEND.
  Sequence<Vertex> Vertices;

  auto operator==(const Polyline &) const -> bool = default;
};
} // namespace DxfIO::Entities

#endif /* DXFIO_ENTITIES_POLYLINE_HPP */
