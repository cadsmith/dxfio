#ifndef DXFIO_ENTITIES_TOLERANCE_HPP
#define DXFIO_ENTITIES_TOLERANCE_HPP

#include "dxfio/api.hpp"
#include "dxfio/boxed.hpp"
#include "dxfio/entities/record.hpp"
#include "dxfio/symbol.hpp"
#include "dxfio/vector.hpp"

#include <string>

namespace DxfIO::Entities
{
  /**
   *  A TOLERANCE: a feature control frame of geometric tolerancing.
   */
  struct DXFIO_API Tolerance
  {
    /**
     *  The layer, linetype, color and the rest every entity carries.
     */
    Entities::Record Record{};
    /**
     *  The name of the dimension style. Group 3.
     */
    Symbol Style{};
    /**
     *  The insertion point, in world coordinates. Groups 10, 20, 30.
     */
    Vector Position{};
    /**
     *  The content of the frame, in the codes AutoCAD writes for its symbols and boxes. Group 1.
     */
    std::string Text{};
    /**
     *  The direction of the frame's X axis, in world coordinates. Groups 11, 21, 31.
     */
    Vector Direction{.X = 1};
    /**
     *  The normal of the plane the entity is drawn in. Groups 210, 220, 230.
     */
    Vector Normal{Up};

    auto operator==(const Tolerance &) const -> bool = default;
  };
} // namespace DxfIO::Entities

#endif /* DXFIO_ENTITIES_TOLERANCE_HPP */
