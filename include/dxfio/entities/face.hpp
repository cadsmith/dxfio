#ifndef DXFIO_ENTITIES_FACE_HPP
#define DXFIO_ENTITIES_FACE_HPP

#include "dxfio/api.hpp"
#include "dxfio/corners.hpp"
#include "dxfio/entities/record.hpp"

namespace DxfIO::Entities
{
  /**
   *  A 3DFACE: a triangle or quadrilateral with its corners anywhere in space.
   */
  struct DXFIO_API Face3d
  {
    /**
     *  The layer, linetype, color and the rest every entity carries.
     */
    Entities::Record Record{};
    /**
     *  The corners, in world coordinates; a triangle repeats its third. Groups 10 to 13, 20 to 23,
     *  30 to 33.
     */
    DxfIO::Corners Corners{};
    /**
     *  Which edges are invisible: 1 the first, 2 the second, 4 the third, 8 the fourth. Group 70.
     */
    int Flags{};

    auto operator==(const Face3d &) const -> bool = default;
  };
} // namespace DxfIO::Entities

#endif /* DXFIO_ENTITIES_FACE_HPP */
