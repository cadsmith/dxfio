#include "encoding/escapes.hpp"

#include "encoding/codepage.hpp"
#include "hex.hpp"

#include <algorithm>
#include <array>
#include <format>
#include <iterator>

namespace DxfIO::Encoding
{
namespace
{
constexpr std::string_view Mark = "\\U+";

constexpr std::string_view Paged = "\\M+";

constexpr std::array<std::string_view, 5> Wides = {
    "ANSI_932", "ANSI_950", "ANSI_949", "ANSI_1361", "ANSI_936",
};

constexpr std::size_t Digits = 4;

constexpr std::size_t Wide = Paged.size() + 1 + Digits;

constexpr unsigned int Byte = 8;

constexpr unsigned int Low8 = 0xff;

constexpr std::size_t Length = Mark.size() + Digits;

constexpr char32_t High = 0xd800;

constexpr char32_t Low = 0xdc00;

constexpr char32_t Past = 0xe000;

constexpr char32_t Plane = 0x10000;

constexpr unsigned int Shift = 10;

constexpr char32_t Bottom = 0x3ff;

auto Single(std::string_view raw) -> std::optional<char32_t>
{
  if (raw.size() < Length || !raw.starts_with(Mark))
  {
    return std::nullopt;
  }
  return Unhex(raw.substr(Mark.size(), Digits));
}

using Numbers = std::array<const Codepage *, Wides.size()>;

auto Numbered(std::size_t at) -> const Codepage &
{
  static const Numbers pages = [] -> Numbers
  {
    Numbers out{};
    std::ranges::transform(Wides, out.begin(), Find);
    return out;
  }();
  return *pages.at(at);
}

auto Multibyte(std::string_view raw) -> std::optional<Escape>
{
  if (raw.size() < Wide || !raw.starts_with(Paged))
  {
    return std::nullopt;
  }
  const char index = raw.at(Paged.size());
  const std::optional<unsigned int> bytes = Unhex(raw.substr(Paged.size() + 1, Digits));
  if (index < '1' || index > '5' || !bytes)
  {
    return std::nullopt;
  }
  const Codepage &page = Numbered(static_cast<std::size_t>(index - '1'));
  const char32_t point =
      Paired(page, static_cast<unsigned char>(*bytes >> Byte), static_cast<unsigned char>(*bytes & Low8));
  if (point == 0)
  {
    return std::nullopt;
  }
  return Escape{.Point = point, .Length = Wide};
}
} // namespace

auto Leading(std::string_view raw) -> std::optional<Escape>
{
  if (const std::optional<Escape> wide = Multibyte(raw))
  {
    return wide;
  }
  const std::optional<char32_t> one = Single(raw);
  if (!one || (*one >= Low && *one < Past))
  {
    return std::nullopt;
  }
  if (*one < High || *one >= Past)
  {
    return Escape{.Point = *one, .Length = Length};
  }
  const std::optional<char32_t> two = Single(raw.substr(Length));
  if (!two || *two < Low || *two >= Past)
  {
    return std::nullopt;
  }
  return Escape{.Point = Plane + ((*one - High) << Shift) + (*two - Low), .Length = 2 * Length};
}

void Escaped(std::string &out, char32_t point)
{
  if (point < Plane)
  {
    std::format_to(std::back_inserter(out), "\\U+{:04X}", static_cast<unsigned>(point));
    return;
  }
  const char32_t rest = point - Plane;
  std::format_to(std::back_inserter(out), "\\U+{:04X}\\U+{:04X}", (High + (rest >> Shift)), (Low + (rest & Bottom)));
}
} // namespace DxfIO::Encoding
