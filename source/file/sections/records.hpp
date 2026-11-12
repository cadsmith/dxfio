#ifndef DXFIO_FILE_SECTIONS_RECORDS_HPP
#define DXFIO_FILE_SECTIONS_RECORDS_HPP

#include "file/record.hpp"
#include "tags/tag.hpp"

#include "dxfio/failure.hpp"

#include <expected>
#include <span>
#include <vector>

namespace DxfIO::File::Sections
{
  auto Records(std::span<const Tags::Tag> body) -> std::expected<std::vector<Record>, Failure>;
} // namespace DxfIO::File::Sections

#endif /* DXFIO_FILE_SECTIONS_RECORDS_HPP */
