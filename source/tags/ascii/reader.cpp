#include "tags/ascii/reader.hpp"

#include "../blanks.hpp"
#include "encoding/carets.hpp"
#include "tags/codes.hpp"
#include "tags/glimpse.hpp"
#include "tags/values.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
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

    /**
     *  Whether the text ends on a caret that opens an escape, as "^ " writes a caret: the blank
     *  after it is its second half, not padding.
     */
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

    constexpr std::string_view Blanks = " \t";

    /**
     *  A code line, blanks then digits then blanks, 0 to 1071, read with no trimming and no parse;
     *  nothing for any other shape, which is refused.
     */
    auto Coded(std::string_view line) -> std::optional<int>
    {
      std::size_t at = 0;
      while (at < line.size() && Blanks.contains(line[at]))
      {
        ++at;
      }
      int         code   = 0;
      std::size_t digits = 0;
      for (; at < line.size() && line[at] >= '0' && line[at] <= '9'; ++at, ++digits)
      {
        code = (code * 10) + (line[at] - '0');
        if (code > Codes::Last)
        {
          return std::nullopt;
        }
      }
      if (digits == 0)
      {
        return std::nullopt;
      }
      while (at < line.size() && Blanks.contains(line[at]))
      {
        ++at;
      }
      return at == line.size() ? std::optional{code} : std::nullopt;
    }

    constexpr std::uint64_t Ones = 0x0101010101010101ULL;

    constexpr std::uint64_t Highs = 0x8080808080808080ULL;

    constexpr std::uint64_t Feeds = 0x0A0A0A0A0A0A0A0AULL;

    constexpr std::uint64_t Returns = 0x0D0D0D0D0D0D0D0DULL;

    /**
     *  Whether any byte of the word is zero.
     */
    auto Zeroed(std::uint64_t word) -> bool
    {
      return ((word - Ones) & ~word & Highs) != 0;
    }

    /**
     *  Where the line ends: the first carriage return or line feed, or the end of the text. Eight
     *  bytes are looked at together, the byte found among them after.
     */
    auto Break(std::string_view rest) -> std::size_t
    {
      std::size_t at = 0;
      while (rest.size() - at >= sizeof(std::uint64_t))
      {
        std::uint64_t word = 0;
        std::memcpy(&word, &rest[at], sizeof word);
        if (Zeroed(word ^ Feeds) || Zeroed(word ^ Returns))
        {
          break;
        }
        at += sizeof word;
      }
      while (at < rest.size() && rest[at] != '\n' && rest[at] != '\r')
      {
        ++at;
      }
      return at;
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
    const std::uint32_t      at   = m_Line + 1;
    const std::string_view   line = Line();
    const std::optional<int> code = Coded(line);
    if (!code)
    {
      const std::string_view head = Trimmed(line);
      if (head.empty())
      {
        return std::unexpected{
            Failure{.Line = at, .What = "blank line where a group code is expected"}};
      }
      return std::unexpected{Failure{
          .Line = at, .What = std::format("group code expected, found \"{}\"", Glimpse(head))}};
    }
    if (m_At >= m_Text.size())
    {
      Tell(Issue{.Line = at,
                 .What =
                     std::format("group {} at the end of the file has no value, left out", *code)});
      return std::optional<Tag>{};
    }
    const std::string_view text = *code == 0 ? Bare(Line(), at + 1) : Line();
    const std::expected<Content, std::string_view> value = Parse(Codes::Of(*code), text);
    if (!value)
    {
      Tell(Issue{
          .Line = at + 1,
          .What = std::format("{} expected for group {}, found \"{}\", left out", value.error(),
                              *code, Glimpse(text)),
      });
      return std::optional<Tag>{};
    }
    if (!Codes::Defined(*code))
    {
      Tell(Issue{.Line = at, .What = std::format("group {} is no DXF group, kept", *code)});
    }
    return std::optional{Tag{.Code = *code, .Line = at, .Value = *value}};
  }

  auto Reader::Line() -> std::string_view
  {
    const std::string_view rest  = m_Text.substr(m_At);
    const std::size_t      end   = Break(rest);
    m_At                        += end + Ending(rest.substr(end));
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
      Tell(Issue{.Line = line,
                 .What = std::format("group 0 holds \"{}\" with blanks, trimmed", Glimpse(text))});
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
