#include "tags/writer.hpp"

#include "match.hpp"
#include "tags/codes.hpp"
#include "tags/real.hpp"
#include "tags/typed.hpp"

#include <algorithm>
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <format>
#include <iterator>
#include <string_view>
#include <utility>
#include <variant>

namespace DxfIO::Tags
{
namespace
{
constexpr std::size_t Most = 127;

constexpr int Escape = 255;

constexpr std::array<int, 8> Widths{6, 6, 9, 18, 6, 6, 6, 6};

constexpr std::string_view Digits = "0123456789ABCDEF";

auto Width(int code) -> int
{
  return Widths.at(std::to_underlying(Codes::Of(code)));
}

auto Hex(std::string_view bytes) -> std::string
{
  std::string out;
  out.reserve(2 * bytes.size());
  for (const char byte : bytes)
  {
    const auto value = static_cast<unsigned char>(byte);
    out.push_back(Digits[value >> 4U]);
    out.push_back(Digits[value & 0xFU]);
  }
  return out;
}

template <class PieceT>
void Pieces(std::string_view all, const PieceT &piece)
{
  const std::size_t count = std::max<std::size_t>(1, (all.size() + Most - 1) / Most);
  for (std::size_t at = 0; at < count; ++at)
  {
    piece(all.substr(at * Most, Most));
  }
}

template <class NumberT>
void Little(std::string &out, NumberT number)
{
  const auto raw = std::bit_cast<std::array<char, sizeof(NumberT)>>(number);
  out.append(raw.begin(), raw.end());
}

void Sized(std::string &out, Codes::Type is, std::int64_t number)
{
  if (is == Codes::Type::Int16)
  {
    Little(out, static_cast<std::uint16_t>(number));
    return;
  }
  if (is == Codes::Type::Int32)
  {
    Little(out, static_cast<std::uint32_t>(number));
    return;
  }
  Little(out, number);
}

auto Spelled(const Content &held) -> std::string
{
  return std::visit(
      DxfIO::Match{
          [](std::string_view one) -> std::string { return std::string{one}; },
          [](const Chunk &one) -> std::string { return Hex(one.Bytes); },
          [](bool one) -> std::string { return one ? "1" : "0"; },
          [](const auto &one) -> std::string { return std::format("{}", one); },
      },
      held);
}

auto Worded(int code) -> bool
{
  const Codes::Type is = Codes::Of(code);
  return is == Codes::Type::Text || is == Codes::Type::Handle;
}
} // namespace

Writer::Writer(std::string &out, Form form) : m_Out(&out), m_Form(form) {}

auto Writer::Put(const Tag &one) -> std::expected<void, Failure>
{
  const std::size_t line = m_Form == Form::Ascii ? m_Line + 2 : m_Line + 1;
  const std::expected<Content, Failure> value = Typed(one.Code, one.Value);
  if (!value)
  {
    return std::unexpected{Failure{.Line = line, .What = value.error().What}};
  }
  if (m_Form != Form::Ascii)
  {
    return Pack(one.Code, *value);
  }
  Spell(one.Code, *value);
  return {};
}

void Writer::Spell(int code, const Content &value)
{
  std::visit(
      DxfIO::Match{
          [this, code](std::string_view text) -> void { Pair(code, text); },
          [this, code](const Chunk &bytes) -> void
          { Pieces(bytes.Bytes, [this, code](std::string_view part) -> void { Pair(code, Hex(part)); }); },
          [this, code](double number) -> void { Pair(code, Real(number).value_or(std::string{})); },
          [this, code](auto number) -> void { Pair(code, std::format("{:>{}}", std::int64_t{number}, Width(code))); },
      },
      value);
}

auto Writer::Pack(int code, const Content &value) -> std::expected<void, Failure>
{
  ++m_Line;
  if (Worded(code))
  {
    Coded(code);
    m_Out->append(Spelled(value));
    m_Out->push_back('\0');
    return {};
  }
  return std::visit(
      DxfIO::Match{
          [this, code](std::string_view) -> std::expected<void, Failure>
          {
            return std::unexpected{Failure{
                .Line = m_Line,
                .What = std::format("group {} holds bytes kept as text", code),
            }};
          },
          [this, code](const Chunk &bytes) -> std::expected<void, Failure>
          {
            Pieces(bytes.Bytes,
                   [this, code](std::string_view part) -> void
                   {
                     Coded(code);
                     m_Out->push_back(static_cast<char>(part.size()));
                     m_Out->append(part);
                   });
            return {};
          },
          [this, code](double number) -> std::expected<void, Failure>
          {
            Coded(code);
            Little(*m_Out, number);
            return {};
          },
          [this, code](bool flag) -> std::expected<void, Failure>
          {
            Coded(code);
            m_Out->push_back(flag ? '\1' : '\0');
            return {};
          },
          [this, code](std::int64_t number) -> std::expected<void, Failure>
          {
            Coded(code);
            Sized(*m_Out, Codes::Of(code), number);
            return {};
          },
      },
      value);
}

void Writer::Coded(int code)
{
  if (m_Form == Form::Wide)
  {
    Little(*m_Out, static_cast<std::uint16_t>(code));
    return;
  }
  if (code < Escape)
  {
    Little(*m_Out, static_cast<std::uint8_t>(code));
    return;
  }
  Little(*m_Out, static_cast<std::uint8_t>(Escape));
  Little(*m_Out, static_cast<std::uint16_t>(code));
}

void Writer::Pair(int code, std::string_view text)
{
  std::format_to(std::back_inserter(*m_Out), "{:>3}\r\n", code);
  m_Out->append(text);
  m_Out->append("\r\n");
  m_Line += 2;
}
} // namespace DxfIO::Tags
