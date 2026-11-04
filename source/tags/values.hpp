#ifndef DXFIO_TAGS_VALUES_HPP
#define DXFIO_TAGS_VALUES_HPP

#include "tags/codes.hpp"
#include "tags/tag.hpp"

#include <expected>
#include <string_view>

namespace DxfIO::Tags
{
// The value of a group as its type reads it, or what the type expected, as "number", when the text is not one.
[[nodiscard]] DXFIO_API auto Parse(Codes::Type is, std::string_view text) -> std::expected<Content, std::string_view>;
} // namespace DxfIO::Tags

#endif /* DXFIO_TAGS_VALUES_HPP */
