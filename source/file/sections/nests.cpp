#include "nests.hpp"

#include "file/cursor.hpp"
#include "located.hpp"

#include <algorithm>
#include <format>
#include <string>
#include <string_view>
#include <utility>

namespace DxfIO::File::Sections
{
  namespace
  {
    struct Kind
    {
      std::string_view Open;
      std::string_view Close;
    };

    /**
     *  How an issue names a nest: by the name its head gives in group 2, else by its kind.
     */
    auto Called(const Record &head, Kind kind) -> std::string
    {
      const auto found = std::ranges::find(head.Tags, 2, &Tags::Tag::Code);
      return found == head.Tags.end() ? std::format("a {}", kind.Open)
                                      : std::string{Tags::Text(*found)};
    }

    auto Taken(Cursor &in, Kind kind, std::vector<Issue> *issues) -> std::expected<Nest, Failure>
    {
      return in.Take(kind.Open).and_then(
          [&in, kind, issues](const Record &head) -> std::expected<Nest, Failure>
          {
            return in.Until(kind.Close, kind.Open)
                .transform(
                    [&in, kind, issues, &head](std::vector<Record> inside) -> Nest
                    {
                      Nest out{.Head = head, .Records = std::move(inside), .End = std::nullopt};
                      if (in.At(kind.Close))
                      {
                        out.End = *in.Take();
                      }
                      else if (issues != nullptr)
                      {
                        issues->push_back(Issue{
                            .Code = IssueCode::Damaged,
                            .Line = Located(head.Tags.front().Line),
                            .What = std::format(
                                "{} of {} missing, closed {}", kind.Close, Called(head, kind),
                                in.Done() ? "at the end of the section"
                                          : std::format("at the next {}", kind.Open)),
                        });
                      }
                      return out;
                    });
          });
    }

    auto All(std::span<const Tags::Tag> body, Kind kind, std::vector<Issue> *issues)
        -> std::expected<std::vector<Nest>, Failure>
    {
      std::vector<Nest> out;
      Cursor            in{body};
      while (!in.Done())
      {
        std::expected<Nest, Failure> one = Taken(in, kind, issues);
        if (!one)
        {
          return std::unexpected{one.error()};
        }
        out.push_back(std::move(*one));
      }
      return out;
    }
  } // namespace

  auto Tables(std::span<const Tags::Tag> body, std::vector<Issue> *issues)
      -> std::expected<std::vector<Nest>, Failure>
  {
    return All(body, Kind{.Open = "TABLE", .Close = "ENDTAB"}, issues);
  }

  auto Blocks(std::span<const Tags::Tag> body, std::vector<Issue> *issues)
      -> std::expected<std::vector<Nest>, Failure>
  {
    return All(body, Kind{.Open = "BLOCK", .Close = "ENDBLK"}, issues);
  }
} // namespace DxfIO::File::Sections
