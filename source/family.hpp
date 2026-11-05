#ifndef DXFIO_FAMILY_HPP
#define DXFIO_FAMILY_HPP

#include "chained.hpp"
#include "fields/context.hpp"
#include "file/record.hpp"
#include "filing.hpp"
#include "seek.hpp"
#include "shapes.hpp"
#include "tags/tag.hpp"
#include "unknown/io.hpp"

#include "dxfio/failure.hpp"
#include "dxfio/issue.hpp"
#include "dxfio/unknown.hpp"
#include "dxfio/version.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <expected>
#include <span>
#include <string_view>
#include <type_traits>
#include <variant>
#include <vector>

namespace DxfIO::Family
{
template <class VariantT>
struct Kind
{
  std::string_view Type;
  VariantT (*Read)(std::span<const File::Record> &rest, const Fields::Reading &with){nullptr};
  std::expected<void, Failure> (*Write)(Fields::Writing &to, const VariantT &one){nullptr};
};

template <class KindT>
inline constexpr bool Ancient = false;

template <class VariantT, class KindT>
auto Taking(std::span<const File::Record> &rest, const Fields::Reading &with) -> VariantT
{
  const File::Record &one = rest.front();
  rest = rest.subspan(1);
  if constexpr (!Ancient<KindT>)
  {
    if (with.Release < Version::R13)
    {
      return Taken<Unknown>(one, with);
    }
  }
  bool whole = true;
  std::vector<Issue> told;
  Fields::Reading quiet = with;
  quiet.Issues = &told;
  auto out = Filled<KindT>(one, quiet, whole);
  if (!whole)
  {
    with.Tell(Issue{
        .Line = one.Tags.front().Line,
        .What = "a record out of its order, kept as it came",
    });
    return Taken<Unknown>(one, with);
  }
  for (Issue &issue : told)
  {
    with.Tell(std::move(issue));
  }
  Follow(out, rest, with);
  return out;
}

template <class VariantT, class KindT>
auto Giving(Fields::Writing &to, const VariantT &one) -> std::expected<void, Failure>
{
  const auto &held = std::get<KindT>(one);
  return Given(to, held).and_then([&to, &held] -> auto { return Follow(to, held); });
}

template <std::size_t Size>
consteval auto Distinct(const std::array<std::string_view, Size> &all) -> bool
{
  for (std::size_t at = 0; at + 1 < Size; ++at)
  {
    if (all.at(at).empty() || std::ranges::count(all, all.at(at)) != 1)
    {
      return false;
    }
  }
  return all.back().empty();
}

template <class... KindsT>
consteval auto Folded(std::type_identity<std::variant<KindsT...>> /*unused*/)
{
  using VariantT = std::variant<KindsT...>;
  static_assert(std::is_same_v<std::variant_alternative_t<sizeof...(KindsT) - 1, VariantT>, Unknown>,
                "a family's last kind keeps what no kind types");
  static_assert(Distinct(std::array{DxfIO::Type<KindsT>...}), "each kind of a family has a type of its own");
  return std::array{Kind<VariantT>{
      .Type = DxfIO::Type<KindsT>,
      .Read = Taking<VariantT, KindsT>,
      .Write = Giving<VariantT, KindsT>,
  }...};
}

template <class VariantT>
inline constexpr auto Kinds = Folded(std::type_identity<VariantT>{});

template <class VariantT>
auto Named(const File::Record &one) -> const Kind<VariantT> &
{
  const std::string_view type = Tags::Text(one.Tags.front());
  const auto &all = Kinds<VariantT>;
  const auto *found = Seek(all, type, &Kind<VariantT>::Type);
  return found == nullptr || type.empty() ? all.back() : *found;
}

template <class VariantT>
auto Read(std::span<const File::Record> all, const Fields::Reading &with) -> std::vector<VariantT>
{
  std::vector<VariantT> out;
  out.reserve(all.size());
  while (!all.empty())
  {
    out.push_back(Named<VariantT>(all.front()).Read(all, with));
  }
  return out;
}

template <class VariantT>
auto Type(const VariantT &one) -> std::string_view
{
  if (const auto *loose = std::get_if<Unknown>(&one))
  {
    return loose->Type.View();
  }
  return Kinds<VariantT>.at(one.index()).Type;
}

template <class VariantT>
auto Write(Fields::Writing &to, std::span<const VariantT> all) -> std::expected<void, Failure>
{
  return Chained(all, [&to](const VariantT &one) -> auto { return Kinds<VariantT>.at(one.index()).Write(to, one); });
}
} // namespace DxfIO::Family

#endif /* DXFIO_FAMILY_HPP */
