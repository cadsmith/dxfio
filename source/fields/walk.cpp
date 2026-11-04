#include "fields/walk.hpp"

#include "tags/codes.hpp"

#include <algorithm>
#include <iterator>
#include <optional>
#include <ranges>

namespace DxfIO::Fields
{
Walk::Walk(std::span<const Tags::Tag> body, const Reading &with)
    : m_Body(body), m_With(with), m_Used(body.size(), false), m_Unfit(body.size(), false),
      m_Fence{.Begin = 0, .End = body.size(), .Cursor = 0}, m_Budget(body.size())
{
}

auto Walk::Take(int code, Test fits) -> const Tags::Tag *
{
  m_Repeats.Asked(m_Fence.Begin, m_Fence.End, code);
  Judge(code, fits);
  if (m_Ordered)
  {
    return Ordered(code);
  }
  const std::optional<std::size_t> found = Chosen(code);
  if (!found)
  {
    return nullptr;
  }
  m_Used.at(*found) = true;
  m_Fence.Cursor = *found + 1;
  return &At(*found);
}

auto Walk::Subclass(std::string_view marker) -> bool
{
  return Subclass(std::span{&marker, 1});
}

auto Walk::Subclass(std::span<const std::string_view> markers) -> bool
{
  m_Fence = Fence{.Begin = 0, .End = m_Body.size(), .Cursor = 0};
  m_Marked = false;
  if (m_With.Release < Version::R13)
  {
    return false;
  }
  m_Repeats.Mark();
  for (std::size_t index = 0; index < m_Body.size(); ++index)
  {
    const auto named = [this, index](std::string_view marker) -> bool
    {
      return Tags::Holds(At(index), Tags::Codes::Marker, marker);
    };
    if (!m_Used.at(index) && std::ranges::any_of(markers, named))
    {
      m_Used.at(index) = true;
      const std::size_t begin = index + 1;
      m_Fence = Fence{.Begin = begin, .End = Closing(begin), .Cursor = begin};
      m_Marked = true;
      return true;
    }
  }
  return false;
}

void Walk::Expect(int code)
{
  m_Repeats.Asked(m_Fence.Begin, m_Fence.End, code);
}

auto Walk::Gather(std::span<const int> codes) -> std::vector<const Tags::Tag *>
{
  std::vector<const Tags::Tag *> found;
  for (const int code : codes)
  {
    Expect(code);
  }
  for (std::size_t index = m_Fence.Begin; index < m_Fence.End; ++index)
  {
    if (!m_Used.at(index) && std::ranges::contains(codes, At(index).Code))
    {
      m_Used.at(index) = true;
      found.push_back(&At(index));
    }
  }
  return found;
}

auto Walk::Later(int code, int other) const -> bool
{
  const std::optional<std::size_t> one = Chosen(code);
  const std::optional<std::size_t> two = Chosen(other);
  return one && (!two || *one > *two);
}

auto Walk::Holds(int code) const -> bool
{
  if (m_Ordered)
  {
    return m_Next < m_Stop && At(m_Next).Code == code && !m_Unfit.at(m_Next);
  }
  return Chosen(code).has_value();
}

void Walk::Narrow(int opener, const Fence &outer)
{
  const std::size_t from = std::clamp(m_Fence.Cursor, outer.Begin, outer.End);
  const std::size_t start = Next(opener, from, outer.End).value_or(outer.End);
  const std::size_t end = Next(opener, start + 1, outer.End).value_or(outer.End);
  m_Fence = Fence{.Begin = start, .End = end, .Cursor = start};
}

void Walk::Restore(const Fence &saved)
{
  m_Fence = saved;
}

auto Walk::Claim(std::int64_t wanted) -> std::size_t
{
  return m_Budget.Claim(wanted);
}

auto Walk::At(std::size_t index) const -> const Tags::Tag &
{
  return *std::next(m_Body.begin(), static_cast<std::ptrdiff_t>(index));
}

void Walk::Judge(int code, Test fits)
{
  if (fits == nullptr)
  {
    return;
  }
  for (std::size_t index = m_Fence.Begin; index < m_Fence.End; ++index)
  {
    const bool unused = !m_Used.at(index) && At(index).Code == code;
    if (unused && !fits(At(index)))
    {
      m_Unfit.at(index) = true;
    }
  }
}

auto Walk::Chosen(int code) const -> std::optional<std::size_t>
{
  return m_Marked ? Last(code) : After(code);
}

auto Walk::Last(int code) const -> std::optional<std::size_t>
{
  for (const std::size_t index : std::views::iota(m_Fence.Begin, m_Fence.End) | std::views::reverse)
  {
    if (!m_Used.at(index) && !m_Unfit.at(index) && At(index).Code == code)
    {
      return index;
    }
  }
  return std::nullopt;
}

auto Walk::After(int code) const -> std::optional<std::size_t>
{
  const std::optional<std::size_t> found = Next(code, m_Fence.Cursor, m_Fence.End);
  return found ? found : Next(code, m_Fence.Begin, std::min(m_Fence.Cursor, m_Fence.End));
}

auto Walk::Next(int code, std::size_t from, std::size_t to) const -> std::optional<std::size_t>
{
  for (std::size_t index = from; index < to; ++index)
  {
    if (!m_Used.at(index) && !m_Unfit.at(index) && At(index).Code == code)
    {
      return index;
    }
  }
  return std::nullopt;
}

auto Walk::Closing(std::size_t from) const -> std::size_t
{
  for (std::size_t index = from; index < m_Body.size(); ++index)
  {
    if (At(index).Code == Tags::Codes::Marker || At(index).Code == Tags::Codes::Embedded)
    {
      return index;
    }
  }
  return m_Body.size();
}
} // namespace DxfIO::Fields
