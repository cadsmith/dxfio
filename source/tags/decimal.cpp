#include "tags/decimal.hpp"

#include <algorithm>
#include <array>
#include <cfloat>
#include <charconv>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <system_error>

namespace DxfIO::Tags
{
namespace
{
static_assert(FLT_EVAL_METHOD == 0 || FLT_EVAL_METHOD == 1, "an exact reading needs doubles rounded as doubles");

// Every power of ten to 10^22, and every whole number to 2^53, is a double exactly, so one product
// or quotient of the two rounds once, to the double nearest the decimal: Clinger's fast path.
constexpr std::array<double, 23> Tens{
    1e0,  1e1,  1e2,  1e3,  1e4,  1e5,  1e6,  1e7,  1e8,  1e9,  1e10, 1e11,
    1e12, 1e13, 1e14, 1e15, 1e16, 1e17, 1e18, 1e19, 1e20, 1e21, 1e22,
};

constexpr std::int64_t Smallest = -22;

constexpr std::int64_t Largest = 22;

constexpr std::uint64_t Exact = std::uint64_t{1} << 53U;

// The most digits a 64 bit integer holds whatever they are.
constexpr int Held = 19;

// An exponent past any double's; a longer one stops here and cannot overflow.
constexpr std::int64_t Far = 100000;

struct Spelled
{
  std::uint64_t Digits{};
  int Significant{};
  std::int64_t Exponent{};
};

auto Digit(char letter) -> bool
{
  return letter >= '0' && letter <= '9';
}

// Takes the digits at the front of rest into number, each moving its exponent by step; true when
// there was one.
auto Run(std::string_view &rest, Spelled &number, int step) -> bool
{
  std::size_t taken = 0;
  for (; taken < rest.size() && Digit(rest[taken]); ++taken)
  {
    if (number.Significant != 0 || rest[taken] != '0')
    {
      number.Digits = (number.Digits * 10) + static_cast<std::uint64_t>(rest[taken] - '0');
      ++number.Significant;
    }
    number.Exponent += step;
  }
  rest.remove_prefix(taken);
  return taken != 0;
}

// Takes the exponent after an E, its sign and digits, into number; true when it has a digit.
auto Scaled(std::string_view &rest, Spelled &number) -> bool
{
  const bool down = rest.starts_with('-');
  if (down || rest.starts_with('+'))
  {
    rest.remove_prefix(1);
  }
  std::size_t taken = 0;
  std::int64_t power = 0;
  for (; taken < rest.size() && Digit(rest[taken]); ++taken)
  {
    power = std::min((power * 10) + (rest[taken] - '0'), Far);
  }
  rest.remove_prefix(taken);
  number.Exponent += down ? -power : power;
  return taken != 0;
}

// The decimal when its digits and its exponent are both small enough to be exact, as nearly every
// real in a drawing is; nothing for any other.
auto Quick(std::string_view word) -> std::optional<double>
{
  std::string_view rest = word;
  const bool negative = rest.starts_with('-');
  if (negative)
  {
    rest.remove_prefix(1);
  }
  Spelled number;
  bool digits = Run(rest, number, 0);
  if (rest.starts_with('.'))
  {
    rest.remove_prefix(1);
    digits = Run(rest, number, -1) || digits;
  }
  if (digits && (rest.starts_with('E') || rest.starts_with('e')))
  {
    rest.remove_prefix(1);
    digits = Scaled(rest, number);
  }
  if (!digits || !rest.empty() || number.Significant > Held || number.Digits > Exact || number.Exponent < Smallest ||
      number.Exponent > Largest)
  {
    return std::nullopt;
  }
  const auto whole = static_cast<double>(number.Digits);
  const double value = number.Exponent < 0 ? whole / Tens.at(static_cast<std::size_t>(-number.Exponent))
                                           : whole * Tens.at(static_cast<std::size_t>(number.Exponent));
  return negative ? -value : value;
}

// Any other decimal, read by the standard library, exact too but slower.
auto Careful(std::string_view word) -> std::optional<double>
{
  const std::string_view body = word.starts_with('-') ? word.substr(1) : word;
  if (body.empty() || (!Digit(body.front()) && body.front() != '.'))
  {
    return std::nullopt;
  }
  double out{};
  const char *last = std::to_address(word.end());
  const std::from_chars_result read =
      std::from_chars(std::to_address(word.begin()), last, out, std::chars_format::general);
  return read.ec == std::errc{} && read.ptr == last ? std::optional{out} : std::nullopt;
}
} // namespace

auto Decimal(std::string_view word) -> std::optional<double>
{
  const std::optional<double> quick = Quick(word);
  return quick ? quick : Careful(word);
}
} // namespace DxfIO::Tags
