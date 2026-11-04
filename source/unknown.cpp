#include "dxfio/unknown.hpp"

#include "fields/owned.hpp"
#include "fields/text.hpp"
#include "head.hpp"
#include "tail.hpp"
#include "unknown/io.hpp"

namespace DxfIO
{
namespace
{
constexpr int Start = 0;
} // namespace

auto Kept(const File::Record &one, const Fields::Reading &with) -> Unknown
{
  Unknown out{.Type = Symbol{one.Name}, .Groups = {}};
  const std::span<const Tags::Tag> body = one.Tags.subspan(1);
  const std::size_t at = Trailing(body);
  const std::vector<Tags::Tag> rest = Head(body.first(at), Heading{.Into = &out.Base}, with);
  out.Groups.reserve(rest.size());
  for (const Tags::Tag &held : rest)
  {
    out.Groups.push_back(Fields::Owned(held, with.Text));
  }
  Tail(body.subspan(at), out.Base, with);
  return out;
}

auto Put(Fields::Writing &to, const Unknown &one) -> std::expected<void, Failure>
{
  return Fields::Word(to, Start, one.Type.View())
      .and_then([&to, &one] -> std::expected<void, Failure> { return Head(to, one.Base, Handled); })
      .and_then([&to, &one] -> std::expected<void, Failure> { return Fields::Put(to, one.Groups); })
      .and_then([&to, &one] -> std::expected<void, Failure> { return Tail(to, one.Base); });
}
} // namespace DxfIO
