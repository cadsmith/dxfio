#include "file/file.hpp"

#include <algorithm>
#include <cstddef>
#include <format>
#include <string_view>

namespace DxfIO::File
{
namespace
{
auto Closes(const Tags::Tag &one) -> bool
{
  return Tags::Holds(one, 0, "ENDSEC");
}

auto Next(std::span<const Tags::Tag> &rest) -> std::expected<Section, Failure>
{
  const Tags::Tag &start = rest.front();
  if (!Tags::Holds(start, 0, "SECTION"))
  {
    return std::unexpected{Failure{
        .Line = start.Line,
        .What = std::format("SECTION expected, found group {}", start.Code),
    }};
  }
  const std::span<const Tags::Tag> after = rest.subspan(1);
  if (after.empty() || after.front().Code != 2)
  {
    return std::unexpected{Failure{.Line = start.Line, .What = "section without a name"}};
  }
  const std::string_view name = Tags::Text(after.front());
  const std::span<const Tags::Tag> inside = after.subspan(1);
  const auto end = std::ranges::find_if(inside, Closes);
  if (end == inside.end())
  {
    return std::unexpected{Failure{.Line = start.Line, .What = std::format("ENDSEC of {} missing", name)}};
  }
  const auto count = static_cast<std::size_t>(end - inside.begin());
  rest = inside.subspan(count + 1);
  return Section{.Name = name, .Body = inside.first(count)};
}
} // namespace

auto Read(std::span<const Tags::Tag> all) -> std::expected<std::vector<Section>, Failure>
{
  std::vector<Section> out;
  std::span<const Tags::Tag> rest = all;
  while (!rest.empty() && !Tags::Holds(rest.front(), 0, "EOF"))
  {
    std::expected<Section, Failure> one = Next(rest);
    if (!one)
    {
      return std::unexpected{one.error()};
    }
    out.push_back(*one);
  }
  return out;
}
} // namespace DxfIO::File
