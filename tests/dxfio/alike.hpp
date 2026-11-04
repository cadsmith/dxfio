#ifndef DXFIO_TESTS_DXF_ALIKE_HPP
#define DXFIO_TESTS_DXF_ALIKE_HPP

#include "tags/codes.hpp"
#include "tags/tag.hpp"
#include "tags/values.hpp"

#include <algorithm>
#include <optional>
#include <ranges>
#include <string_view>
#include <variant>
#include <vector>

namespace DxfIO
{
inline auto Lines(std::string_view text) -> std::vector<std::string_view>
{
  std::vector<std::string_view> out;
  for (const auto line : text | std::views::split('\n'))
  {
    out.emplace_back(line);
  }
  return out;
}

inline auto Real(std::string_view line) -> std::optional<double>
{
  const std::string_view bare = line.ends_with('\r') ? line.substr(0, line.size() - 1) : line;
  const auto read = Tags::Parse(Tags::Codes::Type::Real, bare);
  const double *held = read ? std::get_if<double>(&*read) : nullptr;
  return held != nullptr ? std::optional{*held} : std::nullopt;
}

inline auto Alike(std::string_view ours, std::string_view theirs) -> bool
{
  return std::ranges::equal(Lines(ours), Lines(theirs),
                            [](std::string_view one, std::string_view other) -> bool
                            {
                              const std::optional<double> number = Real(one);
                              return one == other || (number && number == Real(other));
                            });
}
} // namespace DxfIO

#endif /* DXFIO_TESTS_DXF_ALIKE_HPP */
