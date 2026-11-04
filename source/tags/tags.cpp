#include "tags/tags.hpp"

#include "tags/ascii/reader.hpp"
#include "tags/binary/reader.hpp"
#include "tags/sentinel.hpp"

#include <algorithm>
#include <cstddef>
#include <utility>

namespace DxfIO::Tags
{
namespace
{
constexpr int Comment = 999;

template <class ReaderT>
auto Gathered(ReaderT in, std::size_t room) -> std::expected<std::vector<Tag>, Failure>
{
  std::vector<Tag> out;
  out.reserve(room);
  while (!in.Done())
  {
    std::expected<Tag, Failure> one = in.Next();
    if (!one)
    {
      return std::unexpected{std::move(one.error())};
    }
    if (one->Code != Comment)
    {
      out.push_back(*one);
    }
  }
  return out;
}

auto Lines(std::string_view text) -> std::size_t
{
  return (static_cast<std::size_t>(std::ranges::count(text, '\n')) / 2) + 1;
}
} // namespace

auto All(std::string_view bytes) -> std::expected<std::vector<Tag>, Failure>
{
  if (Marked(bytes))
  {
    return Gathered(Binary::Reader{bytes.substr(Sentinel().size())}, 0);
  }
  return Gathered(Ascii::Reader{bytes}, Lines(bytes));
}
} // namespace DxfIO::Tags
