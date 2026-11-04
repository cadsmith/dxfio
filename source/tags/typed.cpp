#include "tags/typed.hpp"

#include "match.hpp"
#include "tags/codes.hpp"
#include "tags/values.hpp"

#include <cmath>
#include <cstdint>
#include <format>
#include <limits>
#include <optional>
#include <string_view>
#include <utility>
#include <variant>

namespace DxfIO::Tags
{
namespace
{
using Converted = std::expected<Content, Failure>;

auto Wholly(const Content &held) -> std::optional<std::int64_t>
{
  return std::visit(
      DxfIO::Match{
          [](std::int64_t one) -> std::optional<std::int64_t> { return one; },
          [](bool one) -> std::optional<std::int64_t> { return one ? 1 : 0; },
          [](double one) -> std::optional<std::int64_t>
          {
            constexpr double Past = 9.2e18;
            return std::isfinite(one) && std::abs(one) < Past ? std::optional{static_cast<std::int64_t>(one)}
                                                              : std::nullopt;
          },
          [](const auto &) -> std::optional<std::int64_t> { return std::nullopt; },
      },
      held);
}

auto Really(const Content &held) -> std::optional<double>
{
  return std::visit(
      DxfIO::Match{
          [](double one) -> std::optional<double> { return one; },
          [](std::int64_t one) -> std::optional<double> { return static_cast<double>(one); },
          [](bool one) -> std::optional<double> { return one ? 1.0 : 0.0; },
          [](const auto &) -> std::optional<double> { return std::nullopt; },
      },
      held);
}

auto Bound(Codes::Type is) -> std::pair<std::int64_t, std::int64_t>
{
  using Long = std::numeric_limits<std::int32_t>;
  using Huge = std::numeric_limits<std::int64_t>;
  if (is == Codes::Type::Int16)
  {
    return {std::numeric_limits<std::int16_t>::min(), std::numeric_limits<std::uint16_t>::max()};
  }
  if (is == Codes::Type::Int32)
  {
    return {Long::min(), Long::max()};
  }
  return {Huge::min(), Huge::max()};
}

auto Refused(int code) -> Failure
{
  return Failure{.Line = 0, .What = std::format("group {} holds no value of its type", code)};
}

auto Whole(int code, Codes::Type is, const Content &value) -> Converted
{
  const std::optional<std::int64_t> number = Wholly(value);
  const auto [low, high] = Bound(is);
  if (!number || *number < low || *number > high)
  {
    return std::unexpected{Refused(code)};
  }
  return is == Codes::Type::Boolean ? Content{*number != 0} : Content{*number};
}

auto Measured(int code, const Content &value) -> Converted
{
  const std::optional<double> number = Really(value);
  if (!number)
  {
    return std::unexpected{Refused(code)};
  }
  if (!std::isfinite(*number))
  {
    return std::unexpected{Failure{.Line = 0, .What = std::format("group {} holds no finite number", code)}};
  }
  return Content{*number};
}
} // namespace

auto Typed(int code, const Content &held) -> std::expected<Content, Failure>
{
  const Codes::Type is = Codes::Of(code);
  if (is == Codes::Type::Text || is == Codes::Type::Handle)
  {
    return std::holds_alternative<double>(held) ? Measured(code, held) : Converted{held};
  }
  const auto *text = std::get_if<std::string_view>(&held);
  const std::optional<Content> parsed = text == nullptr ? std::nullopt : Parse(is, *text);
  const Content &value = parsed ? *parsed : held;
  if (is == Codes::Type::Binary)
  {
    const bool bytes = text != nullptr || std::holds_alternative<Chunk>(value);
    return bytes ? Converted{value} : std::unexpected{Refused(code)};
  }
  if (is == Codes::Type::Real)
  {
    return Measured(code, value);
  }
  return Whole(code, is, value);
}
} // namespace DxfIO::Tags
