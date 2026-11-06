#ifndef DXFIO_TAGS_GLIMPSE_HPP
#define DXFIO_TAGS_GLIMPSE_HPP

#include <string>
#include <string_view>

namespace DxfIO::Tags
{
// The start of a text for a message, cut short when it is long.
auto Glimpse(std::string_view text) -> std::string;
} // namespace DxfIO::Tags

#endif /* DXFIO_TAGS_GLIMPSE_HPP */
