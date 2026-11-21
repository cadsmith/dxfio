#ifndef DXFIO_BLOCKS_IO_HPP
#define DXFIO_BLOCKS_IO_HPP

#include "entities/io.hpp"
#include "fields/context.hpp"
#include "file/section.hpp"

#include "dxfio/api.hpp"
#include "dxfio/blocks/block.hpp"
#include "dxfio/blocks/definition.hpp"
#include "dxfio/blocks/endblk.hpp"
#include "dxfio/failure.hpp"
#include "dxfio/issue.hpp"

#include <expected>
#include <span>
#include <vector>

namespace DxfIO::Blocks
{
  [[nodiscard]] DXFIO_API auto Read(std::span<const File::Nest> all, const Fields::Reading &with)
      -> std::vector<Definition>;
  [[nodiscard]] DXFIO_API auto Write(Fields::Writing &to, std::span<const Definition> all)
      -> std::expected<void, Failure>;
} // namespace DxfIO::Blocks

#endif /* DXFIO_BLOCKS_IO_HPP */
