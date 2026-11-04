#ifndef DXFIO_ENTITIES_XLINE_HPP
#define DXFIO_ENTITIES_XLINE_HPP

#include "dxfio/api.hpp"
#include "dxfio/entities/record.hpp"
#include "dxfio/vector.hpp"

namespace DxfIO::Entities
{
/// An XLINE: a construction line through a point, without end in either direction.
struct DXFIO_API XLine
{
  /// The layer, linetype, color and the rest every entity carries.
  Entities::Record Record{};
  /// A point the line passes through, in world coordinates. Groups 10, 20, 30.
  Vector Start{};
  /// The direction of the line, as a unit vector. Groups 11, 21, 31.
  Vector Direction{Right};

  auto operator==(const XLine &) const -> bool = default;
};
} // namespace DxfIO::Entities

#endif /* DXFIO_ENTITIES_XLINE_HPP */
