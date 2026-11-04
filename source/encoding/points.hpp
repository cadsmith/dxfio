#ifndef DXFIO_ENCODING_POINTS_HPP
#define DXFIO_ENCODING_POINTS_HPP

#include "dxfio/api.hpp"

#include <cstddef>
#include <string>
#include <string_view>

namespace DxfIO::Encoding
{
struct DXFIO_API Step
{
  char32_t Point{};
  std::size_t Length{};
};

[[nodiscard]] DXFIO_API auto First(std::string_view text) -> Step;

DXFIO_API void Put(std::string &out, char32_t point);
} // namespace DxfIO::Encoding

#endif /* DXFIO_ENCODING_POINTS_HPP */
