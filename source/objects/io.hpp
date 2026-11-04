#ifndef DXFIO_OBJECTS_IO_HPP
#define DXFIO_OBJECTS_IO_HPP

#include "fields/context.hpp"
#include "file/record.hpp"

#include "dxfio/api.hpp"
#include "dxfio/failure.hpp"
#include "dxfio/issue.hpp"
#include "dxfio/object.hpp"

#include <expected>
#include <span>
#include <string_view>
#include <vector>

namespace DxfIO::Objects
{
[[nodiscard]] DXFIO_API auto Read(std::span<const File::Record> all, const Fields::Reading &with)
    -> std::vector<Object>;
[[nodiscard]] DXFIO_API auto Write(Fields::Writing &to, std::span<const Object> all) -> std::expected<void, Failure>;
[[nodiscard]] DXFIO_API auto Type(const Object &one) -> std::string_view;
[[nodiscard]] DXFIO_API auto Based(const Object &one) -> const DxfIO::Base &;
} // namespace DxfIO::Objects

#endif /* DXFIO_OBJECTS_IO_HPP */
