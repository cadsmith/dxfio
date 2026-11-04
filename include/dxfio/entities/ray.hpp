#ifndef DXFIO_ENTITIES_RAY_HPP
#define DXFIO_ENTITIES_RAY_HPP

#include "dxfio/api.hpp"
#include "dxfio/entities/record.hpp"
#include "dxfio/vector.hpp"

namespace DxfIO::Entities
{
/// A RAY: a line that starts at a point and runs without end in one direction.
struct DXFIO_API Ray
{
  /// The layer, linetype, color and the rest every entity carries.
  Entities::Record Record{};
  /// The point the ray starts from, in world coordinates. Groups 10, 20, 30.
  Vector Start{};
  /// The direction the ray runs in, as a unit vector. Groups 11, 21, 31.
  Vector Direction{Right};

  auto operator==(const Ray &) const -> bool = default;
};
} // namespace DxfIO::Entities

#endif /* DXFIO_ENTITIES_RAY_HPP */
