#include "tags/values.hpp"

#include "blanks.hpp"
#include "tags/decimal.hpp"

#include <array>
#include <charconv>
#include <cstdint>
#include <expected>
#include <memory>
#include <optional>
#include <system_error>
#include <utility>

namespace DxfIO::Tags
{
namespace
{
constexpr double Beyond = 9223372036854775808.0;

auto Bare(std::string_view text) -> std::string_view
{
  const std::string_view word = Trimmed(text);
  if (!word.starts_with('+'))
  {
    return word;
  }
  const std::string_view rest = word.substr(1);
  return rest.starts_with('+') || rest.starts_with('-') ? std::string_view{} : rest;
}

auto Whole(const std::from_chars_result &read, const char *last) -> bool
{
  return read.ec == std::errc{} && read.ptr == last;
}

auto Integral(std::string_view word) -> std::optional<std::int64_t>
{
  if (word.empty())
  {
    return std::nullopt;
  }
  std::int64_t out{};
  const char *last = std::to_address(word.end());
  if (Whole(std::from_chars(std::to_address(word.begin()), last, out), last))
  {
    return out;
  }
  const std::optional<double> loose = Decimal(word);
  if (!loose || *loose >= Beyond || *loose < -Beyond)
  {
    return std::nullopt;
  }
  return static_cast<std::int64_t>(*loose);
}

using Parsed = std::expected<Content, std::string_view>;

constexpr std::string_view Number = "number";

constexpr std::string_view Integer = "integer";

auto Verbatim(std::string_view text) -> Parsed
{
  return Content{text};
}

auto Measured(std::string_view text) -> Parsed
{
  const std::optional<double> number = Decimal(Bare(text));
  return number ? Parsed{Content{*number}} : std::unexpected{Number};
}

auto Counted(std::string_view text) -> Parsed
{
  const std::optional<std::int64_t> number = Integral(Bare(text));
  return number ? Parsed{Content{*number}} : std::unexpected{Integer};
}

auto Flagged(std::string_view text) -> Parsed
{
  const std::optional<std::int64_t> number = Integral(Bare(text));
  return number ? Parsed{Content{*number != 0}} : std::unexpected{Integer};
}

using Reading = Parsed (*)(std::string_view text);

constexpr std::array<Reading, 8> Readings{
    Verbatim, Counted, Counted, Counted, Measured, Flagged, Verbatim, Verbatim,
};
} // namespace

auto Parse(Codes::Type is, std::string_view text) -> std::expected<Content, std::string_view>
{
  return Readings.at(std::to_underlying(is))(text);
}
} // namespace DxfIO::Tags
