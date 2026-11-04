#include "tail.hpp"

#include "chained.hpp"
#include "fields/owned.hpp"
#include "fields/text.hpp"

#include <algorithm>
#include <format>
#include <string>

namespace DxfIO
{
namespace
{
constexpr int Application = 1001;

auto Known(const Base &into, const std::string &name) -> bool
{
  return std::ranges::any_of(into.Extended, [&name](const Xdata &one) -> bool { return one.Application == name; });
}

} // namespace

auto Trailing(std::span<const Tags::Tag> body) -> std::size_t
{
  const auto found = std::ranges::find(body, Application, &Tags::Tag::Code);
  return static_cast<std::size_t>(found - body.begin());
}

void Tail(std::span<const Tags::Tag> groups, Base &into, const Fields::Reading &with)
{
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
    const std::string name = Encoding::Decode(with.Text, Tags::Text(one));
    dropped = Known(into, name);
    if (dropped)
    {
      with.Tell(Issue{
          .Line = one.Line,
          .What = std::format("extended data of {} given again, the first kept", name),
      });
      continue;
    }
    into.Extended.push_back(Xdata{.Application = name, .Groups = {}});
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
