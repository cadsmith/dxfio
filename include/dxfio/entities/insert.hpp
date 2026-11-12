#ifndef DXFIO_ENTITIES_INSERT_HPP
#define DXFIO_ENTITIES_INSERT_HPP

#include "dxfio/api.hpp"
#include "dxfio/entities/attrib.hpp"
#include "dxfio/entities/record.hpp"
#include "dxfio/entities/sequence.hpp"
#include "dxfio/symbol.hpp"
#include "dxfio/vector.hpp"

#include <optional>

namespace DxfIO::Entities
{
  /**
   *  An INSERT: a block placed in the drawing, scaled and turned, or a grid of it as MINSERT places
   *  it.
   */
  struct DXFIO_API Insert
  {
    /**
     *  The layer, linetype, color and the rest every entity carries.
     */
    Entities::Record Record{};
    /**
     *  Whether ATTRIB entities follow it in the file. Group 66.
     */
    bool Follows{};
    /**
     *  The name of the block. Group 2.
     */
    Symbol Block{};
    /**
     *  The insertion point, in the entity's own coordinate system. Groups 10, 20, 30.
     */
    Vector Position{};
    /**
     *  The scale along each axis. Groups 41, 42, 43.
     */
    Vector Scale{.X = 1, .Y = 1, .Z = 1};
    /**
     *  The rotation, in degrees. Group 50.
     */
    double Rotation{};
    /**
     *  The number of columns of the grid, when the block is placed as one. Group 70.
     */
    std::optional<int> Columns{};
    /**
     *  The number of rows of the grid, when the block is placed as one. Group 71.
     */
    std::optional<int> Rows{};
    /**
     *  The spacing of the grid's columns. Group 44.
     */
    double Across{};
    /**
     *  The spacing of the grid's rows. Group 45.
     */
    double Down{};
    /**
     *  The normal of the plane the entity is drawn in. Groups 210, 220, 230.
     */
    Vector Extrusion{Up};
    /**
     *  The attributes that follow it in the file, closed by a SEQEND.
     */
    Sequence<Attrib> Attributes{};

    auto operator==(const Insert &) const -> bool = default;
  };
} // namespace DxfIO::Entities

#endif /* DXFIO_ENTITIES_INSERT_HPP */
