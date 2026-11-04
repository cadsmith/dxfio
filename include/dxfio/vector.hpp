#ifndef DXFIO_VECTOR_HPP
#define DXFIO_VECTOR_HPP

#include "dxfio/api.hpp"

namespace DxfIO
{
/// A point, or a direction such as an extrusion, in three dimensions.
struct DXFIO_API Vector
{
  /// Along the x axis, in the point's own group code, such as 10.
  double X{};
  /// Along the y axis, in the code ten above the x's, such as 20.
  double Y{};
  /// Along the z axis, in the code twenty above the x's, such as 30; zero for a point given in two dimensions.
  double Z{};

  auto operator==(const Vector &) const -> bool = default;
};

/// The unit x axis: the direction of a RAY or an XLINE, or the major axis of an ELLIPSE, until set.
inline constexpr Vector Right{.X = 1, .Y = 0, .Z = 0};

/// The unit z axis: the extrusion, or normal, of an entity whose file gives none.
inline constexpr Vector Up{.X = 0, .Y = 0, .Z = 1};
} // namespace DxfIO

#endif /* DXFIO_VECTOR_HPP */
