#include "tags/real.hpp"

#include "whole.hpp"

#include <algorithm>
#include <array>
#include <charconv>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <format>
#include <memory>
#include <string_view>
#include <utility>

namespace DxfIO::Tags
{
namespace
{
constexpr int Lowest = -16;

constexpr int Highest = 16;

constexpr std::size_t Fractional = 15;

constexpr std::size_t Room = 32;

struct Shortest
{
  bool Negative{};
  std::string Digits;
  int Exponent{};
};

auto Decomposed(double value) -> Shortest
{
  std::array<char, Room> buffer{};
  const std::to_chars_result written =
      std::to_chars(buffer.data(), std::to_address(buffer.end()), value, std::chars_format::scientific);
  const std::string_view text{buffer.data(), written.ptr};
  const std::size_t power = text.find('e');
  std::string digits{text.substr(0, power)};
  const bool negative = digits.starts_with('-');
  std::erase(digits, '-');
  std::erase(digits, '.');
  std::string_view exponent = text.substr(power + 1);
  if (exponent.starts_with('+'))
  {
    exponent.remove_prefix(1);
  }
  return {.Negative = negative, .Digits = std::move(digits), .Exponent = Whole(exponent).value_or(0)};
}

auto Fixed(const Shortest &number) -> std::string
{
  if (number.Exponent < 0)
  {
    const auto zeros = static_cast<std::size_t>(-number.Exponent - 1);
    return "0." + std::string(zeros, '0') + number.Digits;
  }
  const auto units = static_cast<std::size_t>(number.Exponent) + 1;
  std::string out = number.Digits.substr(0, units);
  out.resize(units, '0');
  if (number.Digits.size() > units)
  {
    return out + "." + number.Digits.substr(units);
  }
  return number.Exponent + 1 < Highest ? out + ".0" : out;
}

auto Exponential(const Shortest &number) -> std::string
{
  std::string fraction = number.Digits.substr(1);
  fraction.resize(std::max(fraction.size(), Fractional), '0');
  return std::format("{}.{}E{}{:02}", number.Digits.front(), fraction, number.Exponent < 0 ? '-' : '+',
                     std::abs(number.Exponent));
}
} // namespace

auto Real(double value) -> std::optional<std::string>
{
  if (!std::isfinite(value))
  {
    return std::nullopt;
  }
  const Shortest number = Decomposed(value);
  const bool plain = number.Exponent >= Lowest && number.Exponent < Highest;
  const std::string body = plain ? Fixed(number) : Exponential(number);
  return number.Negative ? "-" + body : body;
}
} // namespace DxfIO::Tags
