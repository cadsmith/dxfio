#include "fields/points.hpp"

#include "fields/numbers.hpp"

namespace DxfIO::Fields
{
auto Value<Vector>::Read(Walk &in, int code) -> std::optional<Vector>
{
  const std::optional<double> x = Value<double>::Read(in, code);
  const std::optional<double> y = Value<double>::Read(in, code + Axis);
  const std::optional<double> z = Value<double>::Read(in, code + (2 * Axis));
  if (!x && !y && !z)
  {
    return std::nullopt;
  }
  return Vector{.X = x.value_or(0), .Y = y.value_or(0), .Z = z.value_or(0)};
}

auto Value<Vector>::Write(Writing &to, int code, const Vector &held) -> std::expected<void, Failure>
{
  return Value<double>::Write(to, code, held.X)
      .and_then([&] -> std::expected<void, Failure> { return Value<double>::Write(to, code + Axis, held.Y); })
      .and_then([&] -> std::expected<void, Failure> { return Value<double>::Write(to, code + (2 * Axis), held.Z); });
}
} // namespace DxfIO::Fields
