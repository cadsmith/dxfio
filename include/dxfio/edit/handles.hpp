#ifndef DXFIO_EDIT_HANDLES_HPP
#define DXFIO_EDIT_HANDLES_HPP

#include "dxfio/api.hpp"
#include "dxfio/drawing.hpp"
#include "dxfio/failure.hpp"
#include "dxfio/handle.hpp"

#include <expected>

namespace DxfIO::Edit
{
  /**
   *  Takes the handle $HANDSEED names, or the one past every handle held when there is no seed, and
   *  moves $HANDSEED past it. Load raises a seed behind the handles in use; one left behind by
   *  handles set by hand is trusted, so run Audit::Repair on such a drawing first. Fails when the
   *  drawing already holds the last handle there is.
   */
  [[nodiscard]] DXFIO_API auto MakeHandle(Drawing &drawing) -> std::expected<Handle, Failure>;
} // namespace DxfIO::Edit

#endif /* DXFIO_EDIT_HANDLES_HPP */
