#include "braces.hpp"

#include "chained.hpp"
#include "fields/handles.hpp"
#include "fields/owned.hpp"
#include "tags/codes.hpp"

#include <array>
#include <string>
#include <string_view>

namespace DxfIO
{
namespace
{
constexpr int Dictionary = 360;

auto Text(const Fields::Writing &to, std::string_view value) -> std::expected<void, Failure>
{
  const std::string written = Encoding::Encode(to.Text, value);
  return to.To->Put(Tags::Tag{
      .Code = Tags::Codes::Control,
      .Value = std::string_view{written},
  });
}

template <class PutT>
auto Enclosed(Fields::Writing &to, std::string_view name, PutT inside) -> std::expected<void, Failure>
{
  return Text(to, "{" + std::string{name}).and_then(inside).and_then([&to] -> auto { return Text(to, "}"); });
}

auto Handles(Fields::Writing &to, int code, std::span<const Handle> all) -> std::expected<void, Failure>
{
  return Chained(all, [&](const Handle one) -> std::expected<void, Failure>
                 { return Fields::Value<Handle>::Write(to, code, one); });
}

auto Extended(Fields::Writing &to, const Base &from) -> std::expected<void, Failure>
{
  if (!from.Dictionary)
  {
    return {};
  }
  const std::array held{from.Dictionary};
  return Enclosed(to, Extension,
                  [&to, &held] -> std::expected<void, Failure> { return Handles(to, Dictionary, held); });
}

auto Watching(Fields::Writing &to, const Base &from) -> std::expected<void, Failure>
{
  if (from.Reactors.empty())
  {
    return {};
  }
  return Enclosed(to, Reactors,
                  [&to, &from] -> std::expected<void, Failure> { return Handles(to, Reactor, from.Reactors); });
}

auto Applied(Fields::Writing &to, const Base &from) -> std::expected<void, Failure>
{
  return Chained(from.Braces,
                 [&to](const Brace &one) -> std::expected<void, Failure>
                 {
                   return Enclosed(to, one.Name,
                                   [&to, &one] -> std::expected<void, Failure> { return Fields::Put(to, one.Groups); });
                 });
}
} // namespace

auto Braces(Fields::Writing &to, const Base &from) -> std::expected<void, Failure>
{
  return Extended(to, from)
      .and_then([&to, &from] -> std::expected<void, Failure> { return Watching(to, from); })
      .and_then([&to, &from] -> std::expected<void, Failure> { return Applied(to, from); });
}
} // namespace DxfIO
