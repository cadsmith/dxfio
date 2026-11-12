#include "file/cursor.hpp"

#include <algorithm>
#include <format>

namespace DxfIO::File
{
  Cursor::Cursor(std::span<const Tags::Tag> all) : m_All(all) {}

  auto Cursor::Done() const -> bool
  {
    return m_At >= m_All.size();
  }

  auto Cursor::Take() -> std::expected<Record, Failure>
  {
    const std::span<const Tags::Tag> here = Rest();
    if (here.empty())
    {
      return std::unexpected{Failure{.Line = Line(), .What = "group 0 expected, found none"}};
    }
    if (here.front().Code != 0)
    {
      return std::unexpected{Failure{
          .Line = Line(),
          .What = std::format("group 0 expected, found group {}", here.front().Code),
      }};
    }
    const std::span<const Tags::Tag> after  = here.subspan(1);
    const auto                       next   = std::ranges::find(after, 0, &Tags::Tag::Code);
    const auto                       size   = static_cast<std::size_t>(next - after.begin()) + 1;
    m_At                                   += size;
    return Record{.Name = Tags::Text(here.front()), .Tags = here.first(size)};
  }

  auto Cursor::Take(std::string_view name) -> std::expected<Record, Failure>
  {
    const std::size_t              at  = Line();
    std::expected<Record, Failure> one = Take();
    if (one && one->Name != name)
    {
      return std::unexpected{
          Failure{.Line = at, .What = std::format("{} expected, found {}", name, one->Name)}};
    }
    return one;
  }

  auto Cursor::Until(std::string_view end) -> std::expected<std::vector<Record>, Failure>
  {
    std::vector<Record> out;
    while (!Done() && !Tags::Holds(Rest().front(), 0, end))
    {
      std::expected<Record, Failure> one = Take();
      if (!one)
      {
        return std::unexpected{one.error()};
      }
      out.push_back(*one);
    }
    if (Done())
    {
      return std::unexpected{Failure{.Line = Line(), .What = std::format("{} missing", end)}};
    }
    return out;
  }

  auto Cursor::Rest() const -> std::span<const Tags::Tag>
  {
    return m_All.subspan(std::min(m_At, m_All.size()));
  }

  auto Cursor::Line() const -> std::size_t
  {
    if (m_All.empty())
    {
      return 0;
    }
    return Done() ? m_All.back().Line : Rest().front().Line;
  }
} // namespace DxfIO::File
