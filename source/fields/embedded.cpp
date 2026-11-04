#include "fields/embedded.hpp"

#include "tags/codes.hpp"

#include <cstddef>
#include <optional>
#include <string_view>

namespace DxfIO::Fields
{
auto Walk::Embed() -> bool
{
  const std::optional<std::size_t> found = Next(Tags::Codes::Embedded, 0, m_Body.size());
  if (!found)
  {
    return false;
  }
  m_Used.at(*found) = true;
  const std::size_t begin = *found + 1;
  m_Fence = Fence{.Begin = begin, .End = Closing(begin), .Cursor = begin};
  m_Marked = true;
  return true;
}

auto Open(const Writing &to) -> std::expected<void, Failure>
{
  constexpr std::string_view Written = "Embedded Object";
  return to.To->Put(Tags::Tag{.Code = Tags::Codes::Embedded, .Value = Written});
}
} // namespace DxfIO::Fields
