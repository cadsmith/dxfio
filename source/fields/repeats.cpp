#include "fields/repeats.hpp"

#include <algorithm>
#include <cstddef>
#include <ranges>
#include <span>

namespace DxfIO::Fields
{
  /**
   *  Forgets every ask, the room kept for the next record's.
   */
  void Repeats::Clear()
  {
    m_Asked.clear();
    m_Last = 0;
  }

  void Repeats::Asked(std::size_t begin, std::size_t end, int code)
  {
    const Ask one{.Begin = begin, .End = end, .Code = code};
    if (!m_Asked.empty() && m_Asked.back() == one)
    {
      return;
    }
    const auto touching =
        std::span{m_Asked}.subspan(m_Last)
        | std::views::reverse
        | std::views::take_while([begin](const Ask &held) -> bool { return held.End == begin; });
    const auto found = std::ranges::find(touching, code, &Ask::Code);
    if (found == touching.end())
    {
      m_Asked.push_back(one);
      return;
    }
    found->End = end;
  }

  auto Repeats::Repeated(std::size_t at, int code) const -> bool
  {
    return std::ranges::any_of(m_Asked, [at, code](const Ask &one) -> bool
                               { return one.Code == code && one.Begin <= at && at < one.End; });
  }

  void Repeats::Mark()
  {
    m_Last = m_Asked.size();
  }

  auto Repeats::Trails(int code) const -> bool
  {
    return std::ranges::contains(std::span{m_Asked}.subspan(m_Last), code, &Ask::Code);
  }
} // namespace DxfIO::Fields
