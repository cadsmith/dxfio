#ifndef DXFIO_ENTITIES_VERTEX_HPP
#define DXFIO_ENTITIES_VERTEX_HPP

#include "dxfio/api.hpp"
#include "dxfio/entities/record.hpp"
#include "dxfio/flags.hpp"
#include "dxfio/vector.hpp"

#include <cstdint>
#include <optional>

namespace DxfIO::Entities
{
  /**
   *  The bits of a VERTEX's flags, group 70, to test with `Has`.
   */
  enum class VertexFlags : std::uint8_t
  {
    /**
     *  Added by curve fitting.
     */
    CurveFitExtra = 1,
    /**
     *  Has a curve-fit tangent.
     */
    Tangent = 2,
    /**
     *  Added by spline fitting.
     */
    SplineFit = 8,
    /**
     *  A point of the spline frame.
     */
    ControlPoint = 16,
    Polyline3d   = 32, /**< Of a 3D polyline. */
    /**
     *  Of a polygon mesh; with PolyfaceMesh, a polyface mesh's vertex.
     */
    PolygonMesh = 64,
    /**
     *  Alone, a face of a polyface mesh.
     */
    PolyfaceMesh = 128,
  };

  /**
   *  A VERTEX: a point of a POLYLINE, or a face of a polyface mesh.
   */
  struct DXFIO_API Vertex
  {
    /**
     *  The layer, linetype, color and the rest every entity carries.
     */
    Entities::Record Record{};
    /**
     *  The location: in the polyline's own coordinate system for a 2D polyline, in world
     *  coordinates otherwise. Groups 10, 20, 30.
     */
    Vector Position{};
    /**
     *  The width the segment from this vertex starts with, when it has its own. Group 40.
     */
    std::optional<double> StartWidth{};
    /**
     *  The width the segment from this vertex ends with, when it has its own. Group 41.
     */
    std::optional<double> EndWidth{};
    /**
     *  The bulge of the segment to the next vertex: the tangent of a quarter of its arc's angle, 0
     *  when straight. Group 42.
     */
    double Bulge{};
    /**
     *  The vertex identifier. Group 91, written from R2010 as AutoCAD writes it, or in the release
     *  the drawing came from; read from R13.
     */
    int Id{};
    /**
     *  The bits of VertexFlags. Group 70.
     */
    int Flags{};
    /**
     *  The direction of the curve-fit tangent. Group 50.
     */
    double Tangent{};
    /**
     *  The first vertex of a polyface mesh face, counted from 1; negative when the edge from it is
     *  invisible. Group 71.
     */
    int First{};
    /**
     *  The second vertex of a polyface mesh face, counted the same way. Group 72.
     */
    int Second{};
    /**
     *  The third vertex of a polyface mesh face, counted the same way. Group 73.
     */
    int Third{};
    /**
     *  The fourth vertex of a polyface mesh face, counted the same way; 0 for a triangle. Group 74.
     */
    int Fourth{};

    auto operator==(const Vertex &) const -> bool = default;
  };
} // namespace DxfIO::Entities

#endif /* DXFIO_ENTITIES_VERTEX_HPP */
