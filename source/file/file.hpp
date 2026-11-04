#ifndef DXFIO_FILE_FILE_HPP
#define DXFIO_FILE_FILE_HPP

#include "file/section.hpp"
#include "tags/tag.hpp"

#include "dxfio/api.hpp"
#include "dxfio/failure.hpp"

#include <expected>
#include <span>
#include <vector>

namespace DxfIO::File
{
[[nodiscard]] DXFIO_API auto Read(std::span<const Tags::Tag> all) -> std::expected<std::vector<Section>, Failure>;
} // namespace DxfIO::File

#endif /* DXFIO_FILE_FILE_HPP */
