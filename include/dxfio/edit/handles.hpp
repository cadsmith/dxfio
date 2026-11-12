#ifndef DXFIO_EDIT_HANDLES_HPP
#define DXFIO_EDIT_HANDLES_HPP

#include "dxfio/api.hpp"
#include "dxfio/document.hpp"
#include "dxfio/failure.hpp"
#include "dxfio/handle.hpp"

#include <expected>

namespace DxfIO::Edit
{
  /**
   *  Takes a handle no record of the drawing holds, and moves $HANDSEED past it. Fails when the
   *  drawing already holds the last handle there is.
   */
  [[nodiscard]] DXFIO_API auto NewHandle(Document &drawing) -> std::expected<Handle, Failure>;
} // namespace DxfIO::Edit

#endif /* DXFIO_EDIT_HANDLES_HPP */
