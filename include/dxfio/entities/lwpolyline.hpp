#ifndef DXFIO_ENTITIES_LWPOLYLINE_HPP
#define DXFIO_ENTITIES_LWPOLYLINE_HPP

#include "dxfio/api.hpp"
#include "dxfio/entities/record.hpp"
#include "dxfio/vector.hpp"
#include "dxfio/widths.hpp"

#include <optional>
#include <vector>

namespace DxfIO::Entities
{
/// An LWPOLYLINE: a 2D polyline that holds its vertices in itself, from R14.
struct DXFIO_API LwPolyline
{
  /// A point of the polyline, with the widths and bulge of the segment that starts at it.
  struct Vertex
  {
    /// The location, in the polyline's own coordinate system. Groups 10, 20.
    Vector Position{};
    /// The widths the segment starts and ends with, when it has its own. Groups 40, 41.
    std::optional<DxfIO::Widths> Widths;
    /// The bulge of the segment to the next vertex: the tangent of a quarter of its arc's angle, 0 when straight.
    /// Group 42.
    double Bulge{};
    /// The vertex identifier. Group 91, written from R2010 as AutoCAD writes it, or in the release the drawing came
    /// from.
    int Id{};

    auto operator==(const Vertex &) const -> bool = default;
  };

  /// The layer, linetype, color and the rest every entity carries.
  Entities::Record Record{};
  /// 1 closed, 128 linetype drawn continuously around the vertices. Group 70.
  int Flags{};
  /// The width every segment shares, when they share one. Group 43.
  std::optional<double> Width{0.0};
  /// The elevation, along the normal. Group 38.
  double Elevation{};
  /// How far the entity is extruded along its normal. Group 39.
  double Thickness{};
  /// The vertices, in order; group 90 counts them.
  std::vector<Vertex> Vertices;
  /// The normal of the plane the entity is drawn in. Groups 210, 220, 230.
  Vector Extrusion{Up};

  auto operator==(const LwPolyline &) const -> bool = default;
};
} // namespace DxfIO::Entities

#endif /* DXFIO_ENTITIES_LWPOLYLINE_HPP */
