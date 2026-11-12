#include "braces.hpp"
#include "fields/owned.hpp"
#include "tags/codes.hpp"

#include <algorithm>
#include <cstddef>
#include <format>
#include <iterator>
#include <ranges>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

namespace DxfIO
{
  namespace
  {
    constexpr int Deepest = 64;

    auto Closes(const Tags::Tag &one) -> bool
    {
      return Tags::Holds(one, Tags::Codes::Control, "}");
    }

    auto Handled(const Group &one) -> const Handle *
    {
      return std::get_if<Handle>(&one.Value);
    }

    auto Watched(Base &into, std::span<const Group> groups) -> std::size_t
    {
      std::size_t lost = 0;
      for (const Group &one : groups)
      {
        if (const Handle *found = Handled(one))
        {
          into.Reactors.push_back(*found);
          continue;
        }
        ++lost;
      }
      return lost;
    }

    auto Place(Base &into, std::string_view name, std::vector<Group> groups) -> std::size_t
    {
      if (name == Reactors)
      {
        return Watched(into, groups);
      }
      auto          backward = groups | std::views::reverse;
      const auto    last     = std::ranges::find_if(backward, Handled);
      const Handle *handle   = last == backward.end() ? nullptr : Handled(*last);
      if (name == Extension && handle != nullptr)
      {
        const std::size_t lost = groups.size() - (into.Dictionary ? 0 : 1);
        into.Dictionary        = *handle;
        return lost;
      }
      into.Braces.push_back(Brace{.Name = std::string{name}, .Groups = std::move(groups)});
      return 0;
    }

    using Tagged = std::span<const Tags::Tag>;

    auto Ending(Tagged body) -> Tagged::iterator
    {
      int        depth = 0;
      const auto end   = std::ranges::find_if(body,
                                              [&depth](const Tags::Tag &one) -> bool
                                              {
                                              depth += (Opens(one) ? 1 : 0) - (Closes(one) ? 1 : 0);
                                              return depth    == 0
                                                  || depth    > Deepest
                                                  || one.Code == Tags::Codes::Marker;
                                              });
      if (end != body.end() && Closes(*end))
      {
        return end;
      }
      return std::ranges::find_if(body.subspan(1), [](const Tags::Tag &one) -> bool
                                  { return Opens(one) || one.Code == Tags::Codes::Marker; });
    }

    auto Owned(Tagged body, Tagged::iterator end) -> Tagged::iterator
    {
      const bool marked = end != body.end() && end->Code == Tags::Codes::Marker;
      return marked && end - body.begin() > 1 && std::prev(end)->Code == Reactor ? std::prev(end)
                                                                                 : end;
    }

  } // namespace

  auto Opens(const Tags::Tag &one) -> bool
  {
    return one.Code == Tags::Codes::Control && Tags::Text(one).starts_with('{');
  }

  auto Braced(std::span<const Tags::Tag> body, Base &into, const Fields::Reading &with)
      -> std::size_t
  {
    const auto         end    = Ending(body);
    const bool         closed = end != body.end() && Closes(*end);
    const auto         last   = closed ? end : Owned(body, end);
    std::vector<Group> found;
    for (const Tags::Tag &one : std::ranges::subrange(std::next(body.begin()), last))
    {
      found.push_back(Fields::Owned(one, with.Text));
    }
    const std::string name = Encoding::Decode(with.Text, Tags::Text(body.front()).substr(1));
    const std::size_t lost = Place(into, name, std::move(found));
    if (lost > 0)
    {
      with.Tell(Issue{
          .Line = body.front().Line,
          .What = std::format("brace {} lost {} of its groups", name, lost),
      });
    }
    if (!closed)
    {
      with.Tell(Issue{
          .Line = body.front().Line,
          .What = std::format("brace {} not closed", name),
      });
    }
    const auto taken = static_cast<std::size_t>(last - body.begin());
    return closed ? taken + 1 : taken;
  }
} // namespace DxfIO
