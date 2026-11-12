#ifndef DXFIO_EMPTY_LAYOUTS_HPP
#define DXFIO_EMPTY_LAYOUTS_HPP

#include "roots.hpp"

#include "dxfio/handle.hpp"
#include "dxfio/objects/layout.hpp"

#include <vector>

namespace DxfIO::Empty
{
  auto Layouts(const Roots &ids, Handle viewport) -> std::vector<Objects::Layout>;
} // namespace DxfIO::Empty

#endif /* DXFIO_EMPTY_LAYOUTS_HPP */
