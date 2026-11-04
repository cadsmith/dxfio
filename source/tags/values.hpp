#ifndef DXFIO_TAGS_VALUES_HPP
#define DXFIO_TAGS_VALUES_HPP

#include "tags/codes.hpp"
#include "tags/tag.hpp"

#include "dxfio/api.hpp"

#include <optional>
#include <string_view>

namespace DxfIO::Tags
{
[[nodiscard]] DXFIO_API auto Parse(Codes::Type is, std::string_view text) -> std::optional<Content>;
} // namespace DxfIO::Tags

#endif /* DXFIO_TAGS_VALUES_HPP */
