#ifndef DXFIO_EMPTY_SCALES_HPP
#define DXFIO_EMPTY_SCALES_HPP

#include "seed.hpp"

#include "dxfio/objects/scale.hpp"

#include <vector>

namespace DxfIO::Empty
{
  auto Scales(Seed &handles) -> std::vector<Objects::Scale>;
} // namespace DxfIO::Empty

#endif /* DXFIO_EMPTY_SCALES_HPP */
