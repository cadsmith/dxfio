#ifndef DXFIO_TAGS_SENTINEL_HPP
#define DXFIO_TAGS_SENTINEL_HPP

#include "dxfio/api.hpp"

#include <string_view>

namespace DxfIO::Tags
{
  [[nodiscard]] DXFIO_API auto Sentinel() -> std::string_view;

  [[nodiscard]] DXFIO_API auto Marked(std::string_view bytes) -> bool;
} // namespace DxfIO::Tags

#endif /* DXFIO_TAGS_SENTINEL_HPP */
