#include "tags/ascii/reader.hpp"

#include "../blanks.hpp"
#include "../whole.hpp"
#include "tags/codes.hpp"
#include "tags/values.hpp"

#include <algorithm>
#include <cstddef>
#include <format>
#include <optional>
#include <string>
#include <string_view>

namespace DxfIO::Tags::Ascii
{
namespace
{
constexpr std::string_view Mark = "\xef\xbb\xbf";

auto Ending(std::string_view rest) -> std::size_t
{
  if (rest.starts_with("\r\n"))
  {
    return 2;
  }
  return rest.empty() ? 0 : 1;
}

auto Group(std::string_view head) -> std::optional<int>
{
  const std::optional<int> code = Whole(head);
  if (!code || *code < 0 || *code > Codes::Last)
  {
    return std::nullopt;
  }
  return code;
}

auto Noun(Codes::Type is) -> std::string_view
{
  return is == Codes::Type::Real ? "number" : "integer";
}

constexpr std::size_t Echoed = 64;

auto Glimpse(std::string_view line) -> std::string
{
  return line.size() > Echoed ? std::format("{}...", line.substr(0, Echoed)) : std::string{line};
}
} // namespace

Reader::Reader(std::string_view text) : m_Text(text.starts_with(Mark) ? text.substr(Mark.size()) : text) {}

auto Reader::Done() const -> bool
{
  return m_Text.find_first_not_of(" \t\r\n", m_At) == std::string_view::npos;
}

auto Reader::Next() -> std::expected<Tag, Failure>
{
  const std::size_t at = m_Line + 1;
  const std::string_view head = Trimmed(Line());
  if (head.empty())
  {
    return std::unexpected{Failure{.Line = at, .What = "blank line where a group code is expected"}};
  }
  const std::optional<int> code = Group(head);
  if (!code)
  {
    return std::unexpected{
        Failure{.Line = at, .What = std::format("group code expected, found \"{}\"", Glimpse(head))}};
  }
  if (m_At >= m_Text.size())
  {
    return std::unexpected{Failure{.Line = at, .What = "value missing after the last group code"}};
  }
  const std::string_view text = Line();
  const Codes::Type is = Codes::Of(*code);
  const std::optional<Content> value = Parse(is, text);
  if (!value)
  {
    return std::unexpected{Failure{
        .Line = at + 1,
        .What = std::format("{} expected for group {}, found \"{}\"", Noun(is), *code, Glimpse(text)),
    }};
  }
  return Tag{.Code = *code, .Value = *value, .Line = at};
}

auto Reader::Line() -> std::string_view
{
  const std::string_view rest = m_Text.substr(m_At);
  const std::size_t end = std::min(rest.find_first_of("\r\n"), rest.size());
  m_At += end + Ending(rest.substr(end));
  ++m_Line;
  return rest.substr(0, end);
}
} // namespace DxfIO::Tags::Ascii
