#include "head.hpp"

#include "braces.hpp"
#include "fields/handles.hpp"
#include "fields/scratch.hpp"
#include "tags/codes.hpp"

#include "dxfio/handle.hpp"

#include <format>
#include <optional>
#include <utility>

namespace DxfIO
{
namespace
{
constexpr int Owner = 330;

void Replaced(const Tags::Tag &one, Handle held, const Fields::Reading &with)
{
  if (held)
  {
    with.Tell(Issue{
        .Line = one.Line,
        .What = std::format("group {} given again, one kept", one.Code),
    });
  }
}

auto Took(const Tags::Tag &one, const Heading &to, const Fields::Reading &with) -> bool
{
  const std::optional<Handle> found = Parsed(Tags::Text(one));
  const bool modern = with.Release >= Version::R13;
  if (one.Code == to.Code && found && *found)
  {
    Replaced(one, to.Into->Self, with);
    to.Into->Self = *found;
    return true;
  }
  if (one.Code == Owner && modern && found)
  {
    Replaced(one, to.Into->Owner, with);
    to.Into->Owner = *found;
    return true;
  }
  return false;
}
} // namespace

// The rest's room comes from the scratch when one is given, found by the records before.
auto Head(std::span<const Tags::Tag> body, const Heading &to, const Fields::Reading &with) -> std::vector<Tags::Tag>
{
  std::vector<Tags::Tag> rest;
  if (with.Reuse != nullptr)
  {
    rest = std::move(with.Reuse->Rest);
    rest.clear();
  }
  const bool modern = with.Release >= Version::R13;
  bool marked = false;
  while (!body.empty())
  {
    const Tags::Tag &one = body.front();
    if (!marked && modern && Opens(one))
    {
      body = body.subspan(Braced(body, *to.Into, with));
      continue;
    }
    if (marked || !Took(one, to, with))
    {
      marked = marked || one.Code == Tags::Codes::Marker;
      rest.push_back(one);
    }
    body = body.subspan(1);
  }
  return rest;
}

auto Head(Fields::Writing &to, const Base &from, int code) -> std::expected<void, Failure>
{
  const std::expected<void, Failure> named =
      from.Self ? Fields::Value<Handle>::Write(to, code, from.Self) : std::expected<void, Failure>{};
  if (to.Release < Version::R13)
  {
    return named;
  }
  return named.and_then([&] -> auto { return Braces(to, from); })
      .and_then([&to, &from] -> std::expected<void, Failure>
                { return Fields::Value<Handle>::Write(to, Owner, from.Owner); });
}
} // namespace DxfIO
