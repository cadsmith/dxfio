#include "tags/ascii/reader.hpp"

#include "../blanks.hpp"
#include "../whole.hpp"
#include "encoding/carets.hpp"
#include "tags/codes.hpp"
#include "tags/glimpse.hpp"
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

constexpr char Hat = '^';

constexpr char Blank = ' ';

// Whether the text ends on a caret that opens an escape, as "^ " writes a caret: the blank after it is its
// second half, not padding.
auto Escaping(std::string_view text) -> bool
{
  while (!text.empty())
  {
    text.remove_prefix(Encoding::Caret(text) ? 2 : 1);
    if (text.size() == 1)
    {
      return text.front() == Hat;
    }
  }
  return false;
}

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
    Tell(Issue{.Line = at, .What = std::format("group {} at the end of the file has no value, left out", *code)});
    return std::optional<Tag>{};
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
  if (!Codes::Defined(*code))
  {
    Tell(Issue{.Line = at, .What = std::format("group {} is no DXF group, kept", *code)});
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
  std::string_view word = Trimmed(text);
  if (Escaping(word))
  {
    const std::size_t end = static_cast<std::size_t>(word.data() - text.data()) + word.size();
    if (end < text.size() && text.at(end) == Blank)
    {
      word = text.substr(end - word.size(), word.size() + 1);
    }
  }
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
