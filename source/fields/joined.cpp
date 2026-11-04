#include "fields/joined.hpp"

#include "fields/subclass.hpp"
#include "tags/codes.hpp"

#include <cstddef>

namespace DxfIO::Fields
{
void Walk::Join(std::string_view marker)
{
  if (m_Joined == marker)
  {
    return;
  }
  m_Joined = marker;
  const Fence open = m_Fence;
  if (!Subclass(marker))
  {
    return;
  }
  const std::size_t opener = m_Fence.Begin - 1;
  const std::size_t begin = Closing(open.Begin) == opener ? open.Begin : m_Fence.Begin;
  m_Fence = Fence{.Begin = begin, .End = Through(opener, marker), .Cursor = begin};
}

auto Walk::Through(std::size_t from, std::string_view marker) -> std::size_t
{
  std::size_t last = from;
  for (std::size_t index = from; index < m_Body.size(); ++index)
  {
    const Tags::Tag &one = At(index);
    if (one.Code == Tags::Codes::Embedded)
    {
      break;
    }
    if (Tags::Holds(one, Tags::Codes::Marker, marker))
    {
      m_Used.at(index) = true;
      last = index;
    }
  }
  return Closing(last + 1);
}

void Open(const Joined &shape, Walk &in)
{
  in.Join(shape.Marker);
}

auto Mark(const Joined &shape, const Writing &to) -> std::expected<void, Failure>
{
  return Mark(Subclass{.Marker = shape.Marker}, to);
}
} // namespace DxfIO::Fields
