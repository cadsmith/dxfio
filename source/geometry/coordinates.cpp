#include "dxfio/geometry/coordinates.hpp"

#include "geometry/axes.hpp"

namespace DxfIO::Geometry
{
  auto ToWorld(Vector point, Vector extrusion) -> Vector
  {
    const Axes axes = AxesOf(extrusion);
    return Vector{.X = (point.X * axes.X.X) + (point.Y * axes.Y.X) + (point.Z * axes.Z.X),
                  .Y = (point.X * axes.X.Y) + (point.Y * axes.Y.Y) + (point.Z * axes.Z.Y),
                  .Z = (point.X * axes.X.Z) + (point.Y * axes.Y.Z) + (point.Z * axes.Z.Z)};
  }

  auto ToObject(Vector point, Vector extrusion) -> Vector
  {
    /**
     *  The axes are orthonormal, so the inverse is the transpose.
     */
    const Axes axes = AxesOf(extrusion);
    return Vector{.X = Dot(point, axes.X), .Y = Dot(point, axes.Y), .Z = Dot(point, axes.Z)};
  }
} // namespace DxfIO::Geometry
