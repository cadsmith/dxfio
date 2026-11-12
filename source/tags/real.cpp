#include "tags/real.hpp"

#include "whole.hpp"

#include <array>
#include <charconv>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <memory>
#include <span>
#include <string>
#include <string_view>

namespace DxfIO::Tags
{
  namespace
  {
    constexpr int Lowest = -16;

    constexpr int Highest = 16;

    constexpr std::size_t Fractional = 15;

    constexpr std::size_t Room = 32;

    /**
     *  The shortest digits that read back as the value, as to_chars writes them in scientific form:
     *  the leading digit, the fraction after the point, and the exponent, views into the buffer
     *  given.
     */
    struct Shortest
    {
      std::string_view Lead;
      std::string_view Fraction;
      int              Exponent{};
      bool             Negative{};
    };

    auto Decomposed(std::span<char> buffer, double value) -> Shortest
    {
      const std::to_chars_result written = std::to_chars(
          buffer.data(), std::to_address(buffer.end()), value, std::chars_format::scientific);
      std::string_view text{buffer.data(), written.ptr};
      const bool       negative = text.starts_with('-');
      if (negative)
      {
        text.remove_prefix(1);
      }
      const std::size_t      power    = text.find('e');
      const std::string_view mantissa = text.substr(0, power);
      std::string_view       exponent = text.substr(power + 1);
      if (exponent.starts_with('+'))
      {
        exponent.remove_prefix(1);
      }
      return {
          .Lead     = mantissa.substr(0, 1),
          .Fraction = mantissa.size() > 2 ? mantissa.substr(2) : std::string_view{},
          .Exponent = Whole(exponent).value_or(0),
          .Negative = negative,
      };
    }

    /**
     *  The digits laid out around a point, as AutoCAD writes a real: a point always given, except
     *  on a whole number of sixteen digits, which AutoCAD writes without one.
     */
    void Fixed(std::string &out, const Shortest &number)
    {
      if (number.Exponent < 0)
      {
        out += "0.";
        out.append(static_cast<std::size_t>(-number.Exponent - 1), '0');
        out += number.Lead;
        out += number.Fraction;
        return;
      }
      const auto        units  = static_cast<std::size_t>(number.Exponent) + 1;
      const std::size_t count  = 1 + number.Fraction.size();
      out                     += number.Lead;
      if (count > units)
      {
        out += number.Fraction.substr(0, units - 1);
        out += '.';
        out += number.Fraction.substr(units - 1);
        return;
      }
      out += number.Fraction;
      out.append(units - count, '0');
      if (number.Exponent + 1 < Highest)
      {
        out += ".0";
      }
    }

    /**
     *  The exponent is at least sixteen either way here, so it always has its two digits.
     */
    void Exponential(std::string &out, const Shortest &number)
    {
      out += number.Lead;
      out += '.';
      out += number.Fraction;
      if (number.Fraction.size() < Fractional)
      {
        out.append(Fractional - number.Fraction.size(), '0');
      }
      out += 'E';
      out += number.Exponent < 0 ? '-' : '+';
      std::array<char, Room>     digits{};
      const std::to_chars_result written =
          std::to_chars(digits.data(), std::to_address(digits.end()), std::abs(number.Exponent));
      out.append(digits.data(), written.ptr);
    }
  } // namespace

  auto Real(double value) -> std::optional<std::string>
  {
    if (!std::isfinite(value))
    {
      return std::nullopt;
    }
    std::array<char, Room> buffer{};
    const Shortest         number = Decomposed(buffer, value);
    std::string            out;
    if (number.Negative)
    {
      out += '-';
    }
    if (number.Exponent >= Lowest && number.Exponent < Highest)
    {
      Fixed(out, number);
    }
    else
    {
      Exponential(out, number);
    }
    return out;
  }
} // namespace DxfIO::Tags
