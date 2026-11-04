#ifndef DXFIO_HANDSEED_HPP
#define DXFIO_HANDSEED_HPP

#include "dxfio/api.hpp"
#include "dxfio/document.hpp"
#include "dxfio/handle.hpp"

namespace DxfIO
{
// The handle $HANDSEED holds, the first one a new record may take; null when the header has no $HANDSEED, or one
// that holds no handle.
[[nodiscard]] DXFIO_API auto ReadSeed(const Document &drawing) -> Handle;

// Sets $HANDSEED to that handle, its one group 5, and adds it to the end of the header when the header has none.
DXFIO_API void WriteSeed(Document &drawing, Handle next);
} // namespace DxfIO

#endif /* DXFIO_HANDSEED_HPP */
