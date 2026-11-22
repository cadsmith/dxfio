#ifndef DXFIO_PAST_HPP
#define DXFIO_PAST_HPP

#include "dxfio/api.hpp"
#include "dxfio/drawing.hpp"
#include "dxfio/handle.hpp"

namespace DxfIO
{
  [[nodiscard]] DXFIO_API auto Past(const Drawing &from) -> Handle;
} // namespace DxfIO

#endif /* DXFIO_PAST_HPP */
