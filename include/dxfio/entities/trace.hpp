#ifndef DXFIO_ENTITIES_TRACE_HPP
#define DXFIO_ENTITIES_TRACE_HPP

#include "dxfio/api.hpp"
#include "dxfio/corners.hpp"
#include "dxfio/entities/record.hpp"
#include "dxfio/vector.hpp"

namespace DxfIO::Entities
{
  /**
   *  A TRACE: a filled four-sided segment of a wide line, as the TRACE command draws it.
   */
  struct DXFIO_API Trace
  {
    /**
     *  The layer, linetype, color and the rest every entity carries.
     */
    Entities::Record Record{};
    /**
     *  The corners, in the entity's own coordinate system, in the Z order a SOLID takes. Groups 10
     *  to 13, 20 to 23, 30 to 33.
     */
    DxfIO::Corners Corners{};
    /**
     *  How far the entity is extruded along its normal. Group 39.
     */
    double Thickness{};
    /**
     *  The normal of the plane the entity is drawn in. Groups 210, 220, 230.
     */
    Vector Extrusion{Up};

    auto operator==(const Trace &) const -> bool = default;
  };
} // namespace DxfIO::Entities

#endif /* DXFIO_ENTITIES_TRACE_HPP */
