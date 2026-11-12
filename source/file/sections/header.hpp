#ifndef DXFIO_FILE_SECTIONS_HEADER_HPP
#define DXFIO_FILE_SECTIONS_HEADER_HPP

#include "file/section.hpp"
#include "tags/tag.hpp"

#include "dxfio/failure.hpp"

#include <expected>
#include <span>
#include <vector>

namespace DxfIO::File::Sections
{
  auto Header(std::span<const Tags::Tag> body) -> std::expected<std::vector<Variable>, Failure>;
} // namespace DxfIO::File::Sections

#endif /* DXFIO_FILE_SECTIONS_HEADER_HPP */
