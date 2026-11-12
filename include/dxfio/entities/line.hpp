#ifndef DXFIO_ENTITIES_LINE_HPP
#define DXFIO_ENTITIES_LINE_HPP

#include "dxfio/api.hpp"
#include "dxfio/entities/record.hpp"
#include "dxfio/vector.hpp"

namespace DxfIO::Entities
{
  /**
   *  A LINE: a straight segment between two points.
   */
  struct DXFIO_API Line
  {
    /**
     *  The layer, linetype, color and the rest every entity carries.
     */
    Entities::Record Record{};
    /**
     *  How far the entity is extruded along its normal. Group 39.
     */
    double Thickness{};
    /**
     *  The start point, in world coordinates. Groups 10, 20, 30.
     */
    Vector Start{};
    /**
     *  The end point, in world coordinates. Groups 11, 21, 31.
     */
    Vector End{};
    /**
     *  The normal of the plane the entity is drawn in. Groups 210, 220, 230.
     */
    Vector Extrusion{Up};

    auto operator==(const Line &) const -> bool = default;
  };
} // namespace DxfIO::Entities

#endif /* DXFIO_ENTITIES_LINE_HPP */
