#include "fields/sequence.hpp"

#include "fields/subclass.hpp"

#include <algorithm>
#include <array>
#include <cstddef>

namespace DxfIO::Fields
{
namespace
{
constexpr std::array<int, 2> Strays = {38, 39};
} // namespace

void Walk::Sequence(std::string_view marker)
{
  m_Strict = true;
  m_Ordered = Subclass(marker);
  m_Refused = !m_Ordered;
  m_Marked = false;
  m_Next = m_Fence.Begin;
  m_Stop = m_Fence.End;
  Pass();
}

void Walk::Pass()
{
  while (m_Next < m_Stop && std::ranges::contains(Strays, At(m_Next).Code))
  {
    ++m_Next;
  }
}

auto Walk::Ordered(int code) -> const Tags::Tag *
{
  if (m_Next >= m_Stop || At(m_Next).Code != code || m_Unfit.at(m_Next))
  {
    return nullptr;
  }
  m_Used.at(m_Next) = true;
  const Tags::Tag *found = &At(m_Next);
  ++m_Next;
  Pass();
  return found;
}

auto Walk::Whole() const -> bool
{
  if (!m_Strict)
  {
    return !m_Refused;
  }
  for (std::size_t index = 0; index < m_Body.size(); ++index)
  {
    if (!m_Used.at(index) && !std::ranges::contains(Strays, At(index).Code))
    {
      return false;
    }
  }
  return !m_Refused;
}

void Open(const Sequence &shape, Walk &in)
{
  in.Sequence(shape.Marker);
}

auto Mark(const Sequence &shape, const Writing &to) -> std::expected<void, Failure>
{
  return Mark(Subclass{.Marker = shape.Marker}, to);
}
} // namespace DxfIO::Fields
