#include "tail.hpp"

#include "chained.hpp"
#include "fields/owned.hpp"
#include "fields/text.hpp"

#include <algorithm>
#include <format>
#include <string>
#include <unordered_set>
#include <utility>

namespace DxfIO
{
namespace
{
constexpr int Application = 1001;

} // namespace

auto Trailing(std::span<const Tags::Tag> body) -> std::size_t
{
  const auto found = std::ranges::find(body, Application, &Tags::Tag::Code);
  return static_cast<std::size_t>(found - body.begin());
}

void Tail(std::span<const Tags::Tag> groups, Base &into, const Fields::Reading &with)
{
  std::unordered_set<std::string> known;
  bool dropped = false;
  for (const Tags::Tag &one : groups)
  {
    if (one.Code != Application)
    {
      if (!dropped && into.Extended.empty())
      {
        with.Tell(Issue{
            .Line = one.Line,
            .What = std::format("group {} before its application, left out", one.Code),
        });
        continue;
      }
      if (!dropped)
      {
        into.Extended.back().Groups.push_back(Fields::Owned(one, with.Text));
      }
      continue;
    }
    std::string name = Encoding::Decode(with.Text, Tags::Text(one));
    dropped = !known.insert(name).second;
    if (dropped)
    {
      with.Tell(Issue{
          .Line = one.Line,
          .What = std::format("extended data of {} given again, the first kept", name),
      });
      continue;
    }
    into.Extended.push_back(Xdata{.Application = std::move(name), .Groups = {}});
  }
}

auto Tail(Fields::Writing &to, const Base &from) -> std::expected<void, Failure>
{
  return Chained(from.Extended,
                 [&](const Xdata &one) -> auto
                 {
                   return Fields::Value<std::string>::Write(to, Application, one.Application)
                       .and_then([&to, &one] -> std::expected<void, Failure> { return Fields::Put(to, one.Groups); });
                 });
}
} // namespace DxfIO
