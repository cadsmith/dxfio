#ifndef DXFIO_BLOCKS_IO_HPP
#define DXFIO_BLOCKS_IO_HPP

#include "entities/io.hpp"
#include "fields/context.hpp"
#include "file/section.hpp"

#include "dxfio/api.hpp"
#include "dxfio/blocks/begin.hpp"
#include "dxfio/blocks/blocks.hpp"
#include "dxfio/blocks/end.hpp"
#include "dxfio/failure.hpp"
#include "dxfio/issue.hpp"

#include <expected>
#include <span>
#include <vector>

namespace DxfIO::Blocks
{
  [[nodiscard]] DXFIO_API auto Read(std::span<const File::Nest> all, const Fields::Reading &with)
      -> std::vector<Block>;
  [[nodiscard]] DXFIO_API auto Write(Fields::Writing &to, std::span<const Block> all)
      -> std::expected<void, Failure>;
} // namespace DxfIO::Blocks

#endif /* DXFIO_BLOCKS_IO_HPP */
