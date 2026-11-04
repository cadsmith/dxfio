#include "tags/binary/reader.hpp"

#include "tags/codes.hpp"

#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <format>
#include <utility>

namespace DxfIO::Tags::Binary
{
namespace
{
static_assert(std::endian::native == std::endian::little);

constexpr std::string_view Cut = "cut short";

constexpr std::uint8_t Escape = 255;

using Decoded = std::expected<Content, std::string_view>;

auto Take(std::string_view &rest, std::size_t count) -> std::optional<std::string_view>
{
  if (rest.size() < count)
  {
    return std::nullopt;
  }
  const std::string_view piece = rest.substr(0, count);
  rest.remove_prefix(count);
  return piece;
}

template <class NumberT>
auto Little(std::string_view &rest) -> std::optional<NumberT>
{
  const std::optional<std::string_view> piece = Take(rest, sizeof(NumberT));
  if (!piece)
  {
    return std::nullopt;
  }
  std::array<char, sizeof(NumberT)> raw{};
  std::ranges::copy(*piece, raw.begin());
  return std::bit_cast<NumberT>(raw);
}

auto Text(std::string_view &rest) -> Decoded
{
  const std::size_t end = rest.find('\0');
  if (end == std::string_view::npos)
  {
    return std::unexpected{Cut};
  }
  const std::string_view out = rest.substr(0, end);
  rest.remove_prefix(end + 1);
  return Content{out};
}

template <class NumberT>
auto Integer(std::string_view &rest) -> Decoded
{
  const std::optional<NumberT> number = Little<NumberT>(rest);
  if (!number)
  {
    return std::unexpected{Cut};
  }
  return Content{static_cast<std::int64_t>(*number)};
}

auto Measure(std::string_view &rest) -> Decoded
{
  const std::optional<double> number = Little<double>(rest);
  if (!number)
  {
    return std::unexpected{Cut};
  }
  if (!std::isfinite(*number))
  {
    return std::unexpected{"is no finite number"};
  }
  return Content{*number};
}

auto Flag(std::string_view &rest) -> Decoded
{
  const std::optional<std::uint8_t> byte = Little<std::uint8_t>(rest);
  if (!byte)
  {
    return std::unexpected{Cut};
  }
  return Content{*byte != 0};
}

auto Bytes(std::string_view &rest) -> Decoded
{
  const std::optional<std::uint8_t> length = Little<std::uint8_t>(rest);
  const std::optional<std::string_view> piece = length ? Take(rest, *length) : std::nullopt;
  if (!piece)
  {
    return std::unexpected{Cut};
  }
  return Content{Chunk{*piece}};
}

using Decoding = Decoded (*)(std::string_view &rest);

constexpr std::array<Decoding, 8> Decodings{
    Text, Integer<std::int16_t>, Integer<std::int32_t>, Integer<std::int64_t>, Measure, Flag, Text, Bytes,
};

auto Widened(std::uint16_t code) -> int
{
  return int{code};
}
} // namespace

Reader::Reader(std::string_view bytes) : m_Rest(bytes), m_Wide(bytes.starts_with(std::string_view{"\0\0", 2})) {}

auto Reader::Done() const -> bool
{
  return m_Rest.empty();
}

auto Reader::Next() -> std::expected<Tag, Failure>
{
  const std::optional<int> group = Code();
  if (!group)
  {
    return std::unexpected{Failure{.What = "group code cut short"}};
  }
  if (*group > Codes::Last)
  {
    return std::unexpected{Failure{.What = std::format("group code expected, found {}", *group)}};
  }
  const Decoding decode = Decodings.at(std::to_underlying(Codes::Of(*group)));
  const Decoded value = decode(m_Rest);
  if (!value)
  {
    return std::unexpected{Failure{.What = std::format("group {} {}", *group, value.error())}};
  }
  return Tag{.Code = *group, .Value = *value};
}

auto Reader::Code() -> std::optional<int>
{
  if (m_Wide)
  {
    return Little<std::uint16_t>(m_Rest).transform(Widened);
  }
  return Little<std::uint8_t>(m_Rest).and_then(
      [this](std::uint8_t one) -> std::optional<int>
      { return one == Escape ? Little<std::uint16_t>(m_Rest).transform(Widened) : std::optional{Widened(one)}; });
}
} // namespace DxfIO::Tags::Binary
