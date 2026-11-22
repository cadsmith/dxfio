#include "file/file.hpp"

#include "located.hpp"

#include <algorithm>
#include <cstddef>
#include <format>
#include <string_view>
#include <utility>

namespace DxfIO::File
{
  namespace
  {
    auto Closes(const Tags::Tag &one) -> bool
    {
      return Tags::Holds(one, 0, "ENDSEC");
    }

    void Tell(std::vector<Issue> *issues, Issue issue)
    {
      if (issues != nullptr)
      {
        issues->push_back(std::move(issue));
      }
    }

    auto Opens(const Tags::Tag &one) -> bool
    {
      return Tags::Holds(one, 0, "SECTION") || Tags::Holds(one, 0, "EOF");
    }

    /**
     *  Where a section missing its ENDSEC closes: at the tag that opens what follows, or at the end
     *  of the file when none does.
     */
    auto Stop(const Tags::Tag *next) -> std::string_view
    {
      if (next == nullptr)
      {
        return "the end of the file";
      }
      return Tags::Holds(*next, 0, "EOF") ? "EOF" : "the next SECTION";
    }

    /**
     *  A TABLE where a SECTION should open: the tables of an exporter that wrote no TABLES around
     *  them, taken as that section, up to the next one or EOF.
     */
    auto Tabled(std::span<const Tags::Tag> &rest, std::vector<Issue> *issues) -> Section
    {
      Tell(issues, Issue{.Code = IssueCode::Damaged,
                         .Line = Located(rest.front().Line),
                         .What = "TABLE outside a TABLES section, taken as one"});
      const std::span<const Tags::Tag> after = rest.subspan(1);
      const auto                       end   = std::ranges::find_if(after, Opens);
      const std::size_t                count = 1 + static_cast<std::size_t>(end - after.begin());
      const Section                    out{.Name = "TABLES", .Body = rest.first(count)};
      rest = rest.subspan(count);
      return out;
    }

    auto Next(std::span<const Tags::Tag> &rest, std::vector<Issue> *issues)
        -> std::expected<Section, Failure>
    {
      const Tags::Tag &start = rest.front();
      if (Tags::Holds(start, 0, "TABLE"))
      {
        return Tabled(rest, issues);
      }
      if (!Tags::Holds(start, 0, "SECTION"))
      {
        return std::unexpected{Failure{
            .Code = FailureCode::Damaged,
            .Line = Located(start.Line),
            .What = std::format("SECTION expected, found group {}", start.Code),
        }};
      }
      const std::span<const Tags::Tag> after = rest.subspan(1);
      if (after.empty() || after.front().Code != 2)
      {
        return std::unexpected{Failure{.Code = FailureCode::Damaged,
                                       .Line = Located(start.Line),
                                       .What = "section without a name"}};
      }
      const std::string_view           name   = Tags::Text(after.front());
      const std::span<const Tags::Tag> inside = after.subspan(1);
      const auto end   = std::ranges::find_if(inside, [](const Tags::Tag &one) -> bool
                                              { return Closes(one) || Opens(one); });
      const auto count = static_cast<std::size_t>(end - inside.begin());
      if (end != inside.end() && Closes(*end))
      {
        rest = inside.subspan(count + 1);
        return Section{.Name = name, .Body = inside.first(count)};
      }
      const std::string_view where = Stop(end == inside.end() ? nullptr : &*end);
      Tell(issues, Issue{
                       .Code = IssueCode::Damaged,
                       .Line = Located(start.Line),
                       .What = std::format("ENDSEC of {} missing, closed at {}", name, where),
                   });
      rest = inside.subspan(count);
      return Section{.Name = name, .Body = inside.first(count)};
    }
  } // namespace

  auto Read(std::span<const Tags::Tag> all, std::vector<Issue> *issues)
      -> std::expected<std::vector<Section>, Failure>
  {
    std::vector<Section>       out;
    std::span<const Tags::Tag> rest = all;
    while (!rest.empty() && !Tags::Holds(rest.front(), 0, "EOF"))
    {
      std::expected<Section, Failure> one = Next(rest, issues);
      if (!one)
      {
        return std::unexpected{one.error()};
      }
      out.push_back(*one);
    }
    return out;
  }
} // namespace DxfIO::File
