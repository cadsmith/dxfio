#ifndef DXFIO_HEX_HPP
#define DXFIO_HEX_HPP

#include <charconv>
#include <memory>
#include <optional>
#include <string_view>
#include <system_error>

namespace DxfIO
{
  inline auto Unhex(std::string_view figures) -> std::optional<unsigned int>
  {
    constexpr int                Base  = 16;
    unsigned int                 value =  0;
    const char                  *last  = std::to_address(figures.end());
    const std::from_chars_result read =
        std::from_chars(std::to_address(figures.begin()), last, value, Base);
    if (read.ec != std::errc{} || read.ptr != last)
    {
      return std::nullopt;
    }
    return value;
  }
} // namespace DxfIO

#endif /* DXFIO_HEX_HPP */
