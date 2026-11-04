#include "fields/corners.hpp"

#include "chained.hpp"
#include "fields/points.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <ranges>

namespace DxfIO::Fields
{
auto Value<Corners>::Read(Walk &in, int code) -> std::optional<Corners>
{
  const std::array given{
      Value<Vector>::Read(in, code),
      Value<Vector>::Read(in, code + 1),
      Value<Vector>::Read(in, code + 2),
      Value<Vector>::Read(in, code + 3),
  };
  if (std::ranges::none_of(given, [](const auto &one) -> auto { return one.has_value(); }))
  {
    return std::nullopt;
  }
  const Vector third = given.at(2).value_or(Vector{});
  return Corners{
      given.at(0).value_or(Vector{}),
      given.at(1).value_or(Vector{}),
      third,
      given.at(3).value_or(third),
  };
}

auto Value<Corners>::Write(Writing &to, int code, const Corners &held) -> std::expected<void, Failure>
{
  return Chained(std::views::iota(std::size_t{}, held.size()),
                 [&to, code, &held](std::size_t at) -> std::expected<void, Failure>
                 { return Value<Vector>::Write(to, code + static_cast<int>(at), held.at(at)); });
}
} // namespace DxfIO::Fields
