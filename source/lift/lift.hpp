#ifndef DXFIO_LIFT_LIFT_HPP
#define DXFIO_LIFT_LIFT_HPP

#include "dxfio/api.hpp"
#include "dxfio/drawing.hpp"
#include "dxfio/version.hpp"

namespace DxfIO::Lift
{
  /**
   *  A drawing of R10 or R12 made ready for a release from R13 on: every record given a handle
   *  where it has none, a BLOCK_RECORD for each block, the root dictionary and the layouts, the
   *  classes and the ByLayer and ByBlock linetypes of the release, and every owner that follows.
   *  Entities keep their handles; the rest take those the drawing leaves free.
   */
  [[nodiscard]] DXFIO_API auto Lifted(Drawing from, Version release) -> Drawing;
} // namespace DxfIO::Lift

#endif /* DXFIO_LIFT_LIFT_HPP */
