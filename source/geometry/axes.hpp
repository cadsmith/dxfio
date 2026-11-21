#ifndef DXFIO_GEOMETRY_AXES_HPP
#define DXFIO_GEOMETRY_AXES_HPP

#include "dxfio/vector.hpp"

#include <cmath>

namespace DxfIO::Geometry
{
  struct Axes
  {
    Vector X;
    Vector Y;
    Vector Z;
  };

  inline auto Dot(Vector one, Vector other) -> double
  {
    return (one.X * other.X) + (one.Y * other.Y) + (one.Z * other.Z);
  }

  inline auto Cross(Vector one, Vector other) -> Vector
  {
    return Vector{.X = (one.Y * other.Z) - (one.Z * other.Y),
                  .Y = (one.Z * other.X) - (one.X * other.Z),
                  .Z = (one.X * other.Y) - (one.Y * other.X)};
  }

  inline auto Unit(Vector one) -> Vector
  {
    const double length = std::sqrt(Dot(one, one));
    return length == 0 ? Up : Vector{.X = one.X / length, .Y = one.Y / length, .Z = one.Z / length};
  }

  /**
   *  The DXF reference's arbitrary axis algorithm.
   */
  inline auto AxesOf(Vector extrusion) -> Axes
  {
    constexpr double Near    = 1.0 / 64;
    const Vector     normal  = Unit(extrusion);
    const bool       upright = std::abs(normal.X) < Near && std::abs(normal.Y) < Near;
    const Vector     x       = Unit(Cross(upright ? Vector{.X = 0, .Y = 1, .Z = 0} : Up, normal));
    return Axes{.X = x, .Y = Unit(Cross(normal, x)), .Z = normal};
  }
} // namespace DxfIO::Geometry

#endif /* DXFIO_GEOMETRY_AXES_HPP */
