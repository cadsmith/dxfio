#include "encoding/points.hpp"

#include "seek.hpp"

#include <algorithm>
#include <array>
#include <cstdint>

namespace DxfIO::Encoding
{
namespace
{
struct Form
{
  char32_t Least{};
  char32_t Below{};
  std::uint8_t Mask{};
  std::uint8_t Lead{};
  std::size_t Follows{};
};

constexpr std::array Forms{
    Form{.Least = 0, .Below = 0x80, .Mask = 0x80, .Lead = 0x00, .Follows = 0},
    Form{
        .Least = 0x80,
        .Below = 0x800,
        .Mask = 0xe0,
        .Lead = 0xc0,
        .Follows = 1,
    },
    Form{
        .Least = 0x800,
        .Below = 0x10000,
        .Mask = 0xf0,
        .Lead = 0xe0,
        .Follows = 2,
    },
    Form{
        .Least = 0x10000,
        .Below = 0x110000,
        .Mask = 0xf8,
        .Lead = 0xf0,
        .Follows = 3,
    },
};

constexpr std::uint8_t Continued = 0x80;

constexpr std::uint8_t Pattern = 0xc0;

constexpr std::uint8_t Payload = 0x3f;

constexpr std::size_t Bits = 6;

constexpr char32_t Replacement = 0xfffd;

constexpr std::array<char32_t, 2> Surrogates{0xd800, 0xdfff};

auto Shaped(std::uint8_t lead) -> const Form *
{
  return SeekIf(Forms, [lead](const Form &one) -> bool { return (lead & one.Mask) == one.Lead; });
}

auto Fits(char32_t point, const Form &shape) -> bool
{
  const bool surrogate = point >= Surrogates.front() && point <= Surrogates.back();
  return point >= shape.Least && point < shape.Below && !surrogate;
}

constexpr Step Damaged{.Point = Replacement, .Length = 1};
} // namespace

auto First(std::string_view text) -> Step
{
  const auto lead = static_cast<std::uint8_t>(text.front());
  const Form *shape = Shaped(lead);
  if (shape == nullptr || text.size() <= shape->Follows)
  {
    return Damaged;
  }
  char32_t point = lead & static_cast<std::uint8_t>(~shape->Mask);
  for (const char next : text.substr(1, shape->Follows))
  {
    const auto byte = static_cast<std::uint8_t>(next);
    if ((byte & Pattern) != Continued)
    {
      return Damaged;
    }
    point = (point << Bits) | (static_cast<char32_t>(byte) & Payload);
  }
  return Fits(point, *shape) ? Step{.Point = point, .Length = shape->Follows + 1} : Damaged;
}

void Put(std::string &out, char32_t point)
{
  const auto *shape = SeekIf(Forms, [point](const Form &one) -> bool { return point < one.Below; });
  if (shape == nullptr)
  {
    Put(out, Replacement);
    return;
  }
  const std::size_t shift = Bits * shape->Follows;
  out.push_back(static_cast<char>(shape->Lead | (point >> shift)));
  for (std::size_t at = shift; at > 0; at -= Bits)
  {
    out.push_back(static_cast<char>(Continued | ((point >> (at - Bits)) & Payload)));
  }
}
} // namespace DxfIO::Encoding
