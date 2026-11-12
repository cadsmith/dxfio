#ifndef DXFIO_TAGS_WHOLE_HPP
#define DXFIO_TAGS_WHOLE_HPP

#include <optional>
#include <string_view>

namespace DxfIO::Tags
{
  auto Whole(std::string_view word) -> std::optional<int>;
} // namespace DxfIO::Tags

#endif /* DXFIO_TAGS_WHOLE_HPP */
