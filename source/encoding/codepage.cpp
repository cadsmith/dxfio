#include "encoding/codepage.hpp"

#include "ascii.hpp"
#include "pages.hpp"
#include "seek.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <vector>

namespace DxfIO::Encoding
{
namespace
{
constexpr std::string_view Prefix = "ANSI_";

using Table = std::array<Codepage, Generated::Pages.size()>;

auto Carried() -> Table
{
  Table out{};
  for (std::size_t at = 0; at < out.size(); ++at)
  {
    const Generated::Page &one = Generated::Pages.at(at);
    out.at(at) = Codepage{.Name = one.Name, .High = &one.High, .Pairs = one.Pairs, .Index = at};
  }
  return out;
}

struct Back
{
  char16_t Point{};
  std::uint16_t At{};
};

using Reversed = std::vector<Back>;

auto Reverse(const Codepage &page) -> Reversed
{
  Reversed out;
  for (std::size_t at = 0; at < page.Pairs.size(); ++at)
  {
    if (page.Pairs[at] != 0)
    {
      out.push_back(Back{.Point = page.Pairs[at], .At = static_cast<std::uint16_t>(at)});
    }
  }
  std::ranges::stable_sort(out, {}, &Back::Point);
  return out;
}

auto Leading(const Codepage &page, unsigned char byte) -> bool
{
  if (page.Pairs.empty() || byte < Generated::Lead)
  {
    return false;
  }
  const std::size_t row = byte - Generated::Lead;
  return row < Generated::Leads && std::ranges::any_of(
                                       page.Pairs.subspan(row * Generated::Trails, Generated::Trails),
                                       [](char16_t one) -> bool { return one != 0; });
}

auto Backs(const Codepage &page) -> const Reversed &
{
  static std::array<std::once_flag, Generated::Pages.size()> built;
  static std::array<Reversed, Generated::Pages.size()> all;
  std::call_once(built.at(page.Index), [&page] -> void { all.at(page.Index) = Reverse(page); });
  return all.at(page.Index);
}
} // namespace

namespace
{
// A name the DWG format gives a page, and the number of the page it is.
struct Alias
{
  std::string_view Name;
  std::string_view Number;
};

constexpr std::array Aliases{
    Alias{.Name = "BIG5", .Number = "950"},   Alias{.Name = "KSC5601", .Number = "949"},
    Alias{.Name = "JOHAB", .Number = "1361"}, Alias{.Name = "GB2312", .Number = "936"},
    Alias{.Name = "DOS932", .Number = "932"},
};

// The number of the page a name means: after its ANSI_ prefix, or the number a DWG name stands for.
auto Numbered(std::string_view name) -> std::string_view
{
  const auto *alias = SeekIf(Aliases, [name](const Alias &one) -> bool { return DxfIO::Ascii::Same(one.Name, name); });
  if (alias != nullptr)
  {
    return alias->Number;
  }
  const bool prefixed = DxfIO::Ascii::Same(name.substr(0, Prefix.size()), Prefix);
  return prefixed ? name.substr(Prefix.size()) : name;
}
} // namespace

auto Pages() -> std::span<const Codepage>
{
  static const Table all = Carried();
  return all;
}

auto Find(std::string_view name) -> const Codepage *
{
  const std::string_view number = Numbered(name);
  const std::span<const Codepage> all = Pages();
  const auto found = std::ranges::find_if(all, [number](const Codepage &one) -> bool
                                          { return DxfIO::Ascii::Same(one.Name.substr(Prefix.size()), number); });
  return found == all.end() ? nullptr : &*found;
}

auto Standard() -> const Codepage &
{
  static const Codepage &found = *Find("ANSI_1252");
  return found;
}

auto Decoded(const Codepage &page, unsigned char byte) -> char32_t
{
  return byte < Half ? char32_t{byte} : char32_t{page.High->at(byte - Half)};
}

auto Paired(const Codepage &page, unsigned char lead, unsigned char trail) -> char32_t
{
  if (page.Pairs.empty() || lead < Generated::Lead || trail < Generated::Trail)
  {
    return 0;
  }
  const std::size_t row = lead - Generated::Lead;
  const std::size_t column = trail - Generated::Trail;
  if (row >= Generated::Leads || column >= Generated::Trails)
  {
    return 0;
  }
  return page.Pairs[(row * Generated::Trails) + column];
}

auto Doubled(const Codepage &page, char32_t point) -> std::optional<Twin>
{
  if (page.Pairs.empty())
  {
    return std::nullopt;
  }
  const Reversed &all = Backs(page);
  const auto found = std::ranges::lower_bound(all, point, {}, [](const Back &one) -> char32_t { return one.Point; });
  if (found == all.end() || char32_t{found->Point} != point)
  {
    return std::nullopt;
  }
  return Twin{
      static_cast<char>(Generated::Lead + (found->At / Generated::Trails)),
      static_cast<char>(Generated::Trail + (found->At % Generated::Trails)),
  };
}

auto Encoded(const Codepage &page, char32_t point) -> std::optional<char>
{
  if (point < Half)
  {
    return static_cast<char>(point);
  }
  const auto *const found =
      std::ranges::find_if(*page.High, [point](char16_t one) -> bool { return char32_t{one} == point; });
  if (found == page.High->end())
  {
    return std::nullopt;
  }
  const auto at = static_cast<std::size_t>(found - page.High->begin());
  const auto byte = static_cast<unsigned char>(Half + at);
  if (Leading(page, byte))
  {
    return std::nullopt;
  }
  return static_cast<char>(byte);
}
} // namespace DxfIO::Encoding
