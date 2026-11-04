#ifndef DXFIO_FILE_SECTIONS_NESTS_HPP
#define DXFIO_FILE_SECTIONS_NESTS_HPP

#include "file/section.hpp"
#include "tags/tag.hpp"

#include "dxfio/failure.hpp"

#include <expected>
#include <span>
#include <vector>

namespace DxfIO::File::Sections
{
auto Tables(std::span<const Tags::Tag> body) -> std::expected<std::vector<Nest>, Failure>;

auto Blocks(std::span<const Tags::Tag> body) -> std::expected<std::vector<Nest>, Failure>;
} // namespace DxfIO::File::Sections

#endif /* DXFIO_FILE_SECTIONS_NESTS_HPP */
