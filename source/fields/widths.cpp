#include "fields/widths.hpp"

#include "fields/numbers.hpp"

namespace DxfIO::Fields
{
auto Value<Widths>::Read(Walk &in, int code) -> std::optional<Widths>
{
  const std::optional<double> end = Value<double>::Read(in, code + 1);
  if (!end)
  {
    in.Expect(code);
    return std::nullopt;
  }
  return Widths{
      .Start = Value<double>::Read(in, code).value_or(0),
      .End = *end,
  };
}

auto Value<Widths>::Write(Writing &to, int code, const Widths &held) -> std::expected<void, Failure>
{
  return Value<double>::Write(to, code, held.Start)
      .and_then([&to, code, &held] -> std::expected<void, Failure>
                { return Value<double>::Write(to, code + 1, held.End); });
}
} // namespace DxfIO::Fields
