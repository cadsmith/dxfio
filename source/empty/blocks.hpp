#ifndef DXFIO_EMPTY_BLOCKS_HPP
#define DXFIO_EMPTY_BLOCKS_HPP

#include "blocks/io.hpp"
#include "roots.hpp"
#include "seed.hpp"

#include "dxfio/blocks/definition.hpp"

#include <vector>

namespace DxfIO::Empty
{
  auto Blocks(const Roots &ids, Seed &handles) -> std::vector<Blocks::Definition>;
} // namespace DxfIO::Empty

#endif /* DXFIO_EMPTY_BLOCKS_HPP */
