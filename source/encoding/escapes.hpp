#ifndef DXFIO_ENCODING_ESCAPES_HPP
#define DXFIO_ENCODING_ESCAPES_HPP

#include "dxfio/api.hpp"

#include <cstddef>
#include <optional>
#include <string>
#include <string_view>

namespace DxfIO::Encoding
{
  struct DXFIO_API Escape
  {
    char32_t    Point{};
    std::size_t Length{};
  };

  [[nodiscard]] DXFIO_API auto Leading(std::string_view raw) -> std::optional<Escape>;

  DXFIO_API void Escaped(std::string &out, char32_t point);
} // namespace DxfIO::Encoding

#endif /* DXFIO_ENCODING_ESCAPES_HPP */
