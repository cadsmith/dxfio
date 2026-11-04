#ifndef DXFIO_RAW_IO_HPP
#define DXFIO_RAW_IO_HPP

#include "fields/context.hpp"
#include "file/record.hpp"

#include "dxfio/api.hpp"
#include "dxfio/failure.hpp"
#include "dxfio/group.hpp"
#include "dxfio/raw.hpp"

#include <expected>
#include <span>
#include <string>
#include <vector>

namespace DxfIO
{
[[nodiscard]] DXFIO_API auto Copied(std::span<const File::Record> all, const Fields::Reading &with) -> std::vector<Raw>;
[[nodiscard]] DXFIO_API auto Put(Fields::Writing &to, std::span<const Raw> all) -> std::expected<void, Failure>;
} // namespace DxfIO

#endif /* DXFIO_RAW_IO_HPP */
