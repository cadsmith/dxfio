#ifndef DXFIO_GEOMETRY_TRANSFORM_HPP
#define DXFIO_GEOMETRY_TRANSFORM_HPP

#include "dxfio/api.hpp"
#include "dxfio/blocks/definition.hpp"
#include "dxfio/entities/insert.hpp"
#include "dxfio/matrix.hpp"
#include "dxfio/vector.hpp"

namespace DxfIO::Geometry
{
  /**
   *  Gives what carries a point of the block into world coordinates: the block's base point taken
   *  off, the insert's scale and rotation applied, the result moved to its position and taken from
   *  its object coordinate system to the world.
   *
   *  A MINSERT's first copy, at its first column and row. For a block placed inside another block,
   *  compose the inner insert's transform after the outer's: `Compose(outer, inner)`.
   */
  [[nodiscard]] DXFIO_API auto TransformOf(const Entities::Insert   &insert,
                                           const Blocks::Definition &block) -> Matrix;

  /**
   *  Gives the point the transform carries a point to.
   */
  [[nodiscard]] DXFIO_API auto Transform(const Matrix &transform, Vector point) -> Vector;

  /**
   *  Gives the transform that applies the inner one, then the outer.
   */
  [[nodiscard]] DXFIO_API auto Compose(const Matrix &outer, const Matrix &inner) -> Matrix;
} // namespace DxfIO::Geometry

#endif /* DXFIO_GEOMETRY_TRANSFORM_HPP */
