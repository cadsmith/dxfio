#ifndef DXFIO_TAGS_BLANKS_HPP
#define DXFIO_TAGS_BLANKS_HPP

#include <string_view>

namespace DxfIO::Tags
{
  auto Trimmed(std::string_view text) -> std::string_view;
} // namespace DxfIO::Tags

#endif /* DXFIO_TAGS_BLANKS_HPP */
