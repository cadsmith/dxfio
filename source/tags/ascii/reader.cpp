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
#include <utility>

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

constexpr std::size_t Echoed = 64;

auto Glimpse(std::string_view line) -> std::string
{
  return line.size() > Echoed ? std::format("{}...", line.substr(0, Echoed)) : std::string{line};
}
} // namespace

Reader::Reader(std::string_view text, std::vector<Issue> *issues)
    : m_Text(text.starts_with(Mark) ? text.substr(Mark.size()) : text), m_Issues(issues)
{
}

auto Reader::Done() const -> bool
{
  return m_Text.find_first_not_of(" \t\r\n", m_At) == std::string_view::npos;
}

auto Reader::Next() -> std::expected<std::optional<Tag>, Failure>
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
  const std::string_view text = *code == 0 ? Bare(Line(), at + 1) : Line();
  const std::expected<Content, std::string_view> value = Parse(Codes::Of(*code), text);
  if (!value)
  {
    Tell(Issue{
        .Line = at + 1,
        .What = std::format("{} expected for group {}, found \"{}\", left out", value.error(), *code, Glimpse(text)),
    });
    return std::optional<Tag>{};
  }
  return std::optional{Tag{.Code = *code, .Value = *value, .Line = at}};
}

auto Reader::Line() -> std::string_view
{
  const std::string_view rest = m_Text.substr(m_At);
  const std::size_t end = std::min(rest.find_first_of("\r\n"), rest.size());
  m_At += end + Ending(rest.substr(end));
  ++m_Line;
  return rest.substr(0, end);
}

auto Reader::Bare(std::string_view text, std::size_t line) -> std::string_view
{
  const std::string_view word = Trimmed(text);
  if (word.size() != text.size())
  {
    Tell(Issue{.Line = line, .What = std::format("group 0 holds \"{}\" with blanks, trimmed", Glimpse(text))});
  }
  return word;
}

void Reader::Tell(Issue issue)
{
  if (m_Issues != nullptr)
  {
    m_Issues->push_back(std::move(issue));
  }
}
} // namespace DxfIO::Tags::Ascii
