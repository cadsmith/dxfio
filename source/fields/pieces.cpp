#include "fields/pieces.hpp"

#include "encoding/carets.hpp"
#include "encoding/codepage.hpp"
#include "encoding/escapes.hpp"
#include "encoding/points.hpp"

#include "dxfio/version.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <optional>
#include <string_view>

namespace DxfIO::Fields
{
namespace
{
constexpr int Piece = 3;

constexpr int Last = 1;

constexpr std::array<int, 2> Codes = {Piece, Last};

constexpr std::size_t Most = 250;

struct Stride
{
  std::size_t Bytes{};
  std::size_t Counts{};
};

using Measure = Stride (*)(std::string_view rest, const Encoding::Codepage *page);

auto Bytes(std::string_view rest) -> std::size_t
{
  return Encoding::Caret(rest) ? 2 : 1;
}

// The stride of an escape at the front, \U+XXXX or \M+nXXXX, which no piece may cut: its bytes, counting as many.
auto Escaped(std::string_view rest) -> std::optional<Stride>
{
  const std::optional<Encoding::Escape> found = Encoding::Leading(rest);
  if (!found)
  {
    return std::nullopt;
  }
  return Stride{.Bytes = found->Length, .Counts = found->Length};
}

auto Characters(std::string_view rest, const Encoding::Codepage * /*unused*/) -> Stride
{
  if (const std::optional<Stride> escape = Escaped(rest))
  {
    return *escape;
  }
  return Stride{.Bytes = Encoding::Caret(rest) ? 2 : Encoding::First(rest).Length, .Counts = 1};
}

auto Doubles(std::string_view rest, const Encoding::Codepage * /*unused*/) -> Stride
{
  if (const std::optional<Stride> escape = Escaped(rest))
  {
    return *escape;
  }
  constexpr auto High = static_cast<unsigned char>(0x80);
  const bool pair = rest.size() > 1 && static_cast<unsigned char>(rest.front()) >= High;
  const std::size_t bytes = pair ? 2 : Bytes(rest);
  return Stride{.Bytes = bytes, .Counts = bytes};
}

auto Pairs(std::string_view rest, const Encoding::Codepage *page) -> Stride
{
  if (const std::optional<Stride> escape = Escaped(rest))
  {
    return *escape;
  }
  const bool pair = rest.size() > 1 && Encoding::Paired(*page, static_cast<unsigned char>(rest.front()),
                                                        static_cast<unsigned char>(rest.at(1))) != 0;
  const std::size_t bytes = pair ? 2 : Bytes(rest);
  return Stride{.Bytes = bytes, .Counts = bytes};
}

auto Unit(const Writing &to) -> Measure
{
  if (to.Release >= Version::R2007)
  {
    return Characters;
  }
  return to.Text.Page == nullptr ? Doubles : Pairs;
}

auto Cut(std::string_view rest, Measure by, const Encoding::Codepage *page) -> std::size_t
{
  std::size_t at = 0;
  for (std::size_t held = 0; at < rest.size();)
  {
    const Stride next = by(rest.substr(at), page);
    if (held + next.Counts > Most)
    {
      break;
    }
    held += next.Counts;
    at += next.Bytes;
  }
  return std::min(at, rest.size());
}

auto Put(const Writing &to, int code, std::string_view text) -> std::expected<void, Failure>
{
  return to.To->Put(Tags::Tag{.Code = code, .Value = text});
}
} // namespace

auto Gathered(Walk &in) -> std::string
{
  std::string raw;
  for (const Tags::Tag *one : in.Gather(Codes))
  {
    raw += Tags::Text(*one);
  }
  return Encoding::Decode(in.With().Text, raw);
}

auto Split(Writing &to, const std::string &text) -> std::expected<void, Failure>
{
  const Measure by = Unit(to);
  const Encoding::Codepage *page = to.Text.Page;
  const std::string written = Encoding::Encode(to.Text, text);
  std::string_view rest = written;
  std::expected<void, Failure> done{};
  for (std::size_t end = Cut(rest, by, page); end < rest.size(); end = Cut(rest, by, page))
  {
    const std::string_view piece = rest.substr(0, end);
    done = done.and_then([&to, piece] -> auto { return Put(to, Piece, piece); });
    rest.remove_prefix(end);
  }
  return done.and_then([&to, rest] -> auto { return Put(to, Last, rest); });
}
} // namespace DxfIO::Fields
