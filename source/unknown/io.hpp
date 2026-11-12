#ifndef DXFIO_UNKNOWN_IO_HPP
#define DXFIO_UNKNOWN_IO_HPP

#include "fields/context.hpp"
#include "file/record.hpp"

#include "dxfio/api.hpp"
#include "dxfio/base.hpp"
#include "dxfio/failure.hpp"
#include "dxfio/group.hpp"
#include "dxfio/issue.hpp"
#include "dxfio/unknown.hpp"

#include <expected>
#include <span>
#include <string>
#include <vector>

namespace DxfIO
{
  [[nodiscard]] DXFIO_API auto Kept(const File::Record &one, const Fields::Reading &with)
      -> Unknown;
  [[nodiscard]] DXFIO_API auto Put(Fields::Writing &to, const Unknown &one)
      -> std::expected<void, Failure>;
} // namespace DxfIO

#endif /* DXFIO_UNKNOWN_IO_HPP */
