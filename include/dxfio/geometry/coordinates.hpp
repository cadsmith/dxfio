#ifndef DXFIO_GEOMETRY_COORDINATES_HPP
#define DXFIO_GEOMETRY_COORDINATES_HPP

#include "dxfio/api.hpp"
#include "dxfio/vector.hpp"

namespace DxfIO::Geometry
{
  /**
   *  Gives a point of the object coordinate system an extrusion sets in world coordinates.
   *
   *  The axes follow the DXF reference's arbitrary axis algorithm: when the normal's X and Y are
   *  both under 1/64, the X axis is the world Y axis crossed with the normal, otherwise the world Z
   *  axis crossed with it; Y is the normal crossed with X. A zero extrusion counts as the world Z
   *  axis.
   */
  [[nodiscard]] DXFIO_API auto ToWorld(Vector point, Vector extrusion) -> Vector;

  /**
   *  Gives a point in world coordinates in the object coordinate system an extrusion sets: ToWorld
   *  undone.
   */
  [[nodiscard]] DXFIO_API auto ToObject(Vector point, Vector extrusion) -> Vector;
} // namespace DxfIO::Geometry

#endif /* DXFIO_GEOMETRY_COORDINATES_HPP */
