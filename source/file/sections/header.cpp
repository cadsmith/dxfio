#include "header.hpp"

#include <algorithm>
#include <cstddef>
#include <format>

namespace DxfIO::File::Sections
{
namespace
{
constexpr int Name = 9;
} // namespace

auto Header(std::span<const Tags::Tag> body) -> std::expected<std::vector<Variable>, Failure>
{
  std::vector<Variable> out;
  std::span<const Tags::Tag> rest = body;
  while (!rest.empty())
  {
    const Tags::Tag &head = rest.front();
    if (head.Code != Name)
    {
      return std::unexpected{Failure{
          .Line = head.Line,
          .What = std::format("header variable expected, found group {}", head.Code),
      }};
    }
    const std::span<const Tags::Tag> after = rest.subspan(1);
    const auto end = std::ranges::find(after, Name, &Tags::Tag::Code);
    const auto count = static_cast<std::size_t>(end - after.begin());
    out.push_back(Variable{.Name = Tags::Text(head), .Values = after.first(count)});
    rest = after.subspan(count);
  }
  return out;
}
} // namespace DxfIO::File::Sections
