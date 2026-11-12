#include "dxfio/handle.hpp"

#include "tags/blanks.hpp"

#include <charconv>
#include <format>
#include <memory>
#include <system_error>

namespace DxfIO
{
  namespace
  {
    constexpr std::size_t Digits = 16;

    constexpr int Base = 16;
  } // namespace

  auto Hex(Handle one) -> std::string
  {
    return std::format("{:X}", one.Value);
  }

  auto Parsed(std::string_view text) -> std::optional<Handle>
  {
    const std::string_view word = Tags::Trimmed(text);
    if (word.empty() || word.size() > Digits)
    {
      return std::nullopt;
    }
    std::uint64_t                value = 0;
    const char                  *last  = std::to_address(word.end());
    const std::from_chars_result read =
        std::from_chars(std::to_address(word.begin()), last, value, Base);
    if (read.ec != std::errc{} || read.ptr != last)
    {
      return std::nullopt;
    }
    return Handle{value};
  }
} // namespace DxfIO
