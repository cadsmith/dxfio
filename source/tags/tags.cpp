#include "tags/tags.hpp"

#include "tags/ascii/reader.hpp"
#include "tags/binary/reader.hpp"
#include "tags/sentinel.hpp"

#include <algorithm>
#include <cstddef>
#include <optional>
#include <utility>

namespace DxfIO::Tags
{
  namespace
  {
    constexpr int Comment = 999;

    auto Ends(const Tag &one) -> bool
    {
      return Holds(one, 0, "EOF");
    }

    /**
     *  The line what follows the EOF tag starts on: two past the tag's in ASCII, none in binary.
     */
    auto Following(const Tag &eof) -> std::size_t
    {
      return eof.Line == 0 ? 0 : eof.Line + 2;
    }

    template <class ReaderT>
    auto Gathered(ReaderT in, std::size_t room, std::vector<Issue> *issues)
        -> std::expected<std::vector<Tag>, Failure>
    {
      std::vector<Tag> out;
      out.reserve(room);
      while (!in.Done())
      {
        std::expected<std::optional<Tag>, Failure> one = in.Next();
        if (!one)
        {
          return std::unexpected{std::move(one.error())};
        }
        if (!*one || (*one)->Code == Comment)
        {
          continue;
        }
        out.push_back(**one);
        if (Ends(out.back()))
        {
          break;
        }
      }
      if (!in.Done() && issues != nullptr)
      {
        issues->push_back(
            Issue{.Line = Following(out.back()), .What = "what follows EOF, left out"});
      }
      return out;
    }

    auto Lines(std::string_view text) -> std::size_t
    {
      return (static_cast<std::size_t>(std::ranges::count(text, '\n')) / 2) + 1;
    }
  } // namespace

  auto All(std::string_view bytes, std::vector<Issue> *issues)
      -> std::expected<std::vector<Tag>, Failure>
  {
    if (Marked(bytes))
    {
      return Gathered(Binary::Reader{bytes.substr(Sentinel().size()), issues}, 0, issues);
    }
    return Gathered(Ascii::Reader{bytes, issues}, Lines(bytes), issues);
  }
} // namespace DxfIO::Tags
