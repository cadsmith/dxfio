#include "whole.hpp"

#include <charconv>
#include <memory>
#include <system_error>

namespace DxfIO::Tags
{
  auto Whole(std::string_view word) -> std::optional<int>
  {
    if (word.empty())
    {
      return std::nullopt;
    }
    int                          out  = 0;
    const char                  *last = std::to_address(word.end());
    const std::from_chars_result read = std::from_chars(std::to_address(word.begin()), last, out);
    if (read.ec != std::errc{} || read.ptr != last)
    {
      return std::nullopt;
    }
    return out;
  }
} // namespace DxfIO::Tags
