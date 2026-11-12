#ifndef DXFIO_FAMILY_HPP
#define DXFIO_FAMILY_HPP

#include "chained.hpp"
#include "fields/context.hpp"
#include "fields/scratch.hpp"
#include "file/record.hpp"
#include "filing.hpp"
#include "parallel.hpp"
#include "seek.hpp"
#include "shapes.hpp"
#include "tags/tag.hpp"
#include "tags/writer.hpp"
#include "unknown/io.hpp"

#include "dxfio/failure.hpp"
#include "dxfio/issue.hpp"
#include "dxfio/unknown.hpp"
#include "dxfio/version.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <expected>
#include <iterator>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
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

  /**
   *  Whether a record of that name trails the one before it, taken by that one's reading rather
   *  than read on its own: none in a family as given, the entities say which of theirs.
   */
  template <class VariantT>
  auto Trails(std::string_view /*name*/) -> bool
  {
    return false;
  }

  /**
   *  The fewest records a part read or written on a thread of its own holds, the work of a thread's
   *  start.
   */
  inline constexpr std::size_t Least = 256;

  template <class VariantT, class KindT>
  auto Taking(std::span<const File::Record> &rest, const Fields::Reading &with) -> VariantT
  {
    const File::Record &one = rest.front();
    rest                    = rest.subspan(1);
    if constexpr (!Ancient<KindT>)
    {
      if (with.Release < Version::R13)
      {
        return Taken<Unknown>(one, with);
      }
    }
    bool               whole = true;
    std::vector<Issue> told;
    Fields::Reading    quiet = with;
    quiet.Issues             = &told;
    auto out                 = Filled<KindT>(one, quiet, whole);
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
    static_assert(
        std::is_same_v<std::variant_alternative_t<sizeof...(KindsT) - 1, VariantT>, Unknown>,
        "a family's last kind keeps what no kind types");
    static_assert(Distinct(std::array{DxfIO::Type<KindsT>...}),
                  "each kind of a family has a type of its own");
    return std::array{Kind<VariantT>{
        .Type  = DxfIO::Type<KindsT>,
        .Read  = Taking<VariantT, KindsT>,
        .Write = Giving<VariantT, KindsT>,
    }...};
  }

  template <class VariantT>
  inline constexpr auto Kinds = Folded(std::type_identity<VariantT>{});

  template <class VariantT>
  auto Named(const File::Record &one) -> const Kind<VariantT> &
  {
    const std::string_view type  = Tags::Text(one.Tags.front());
    const auto            &all   = Kinds<VariantT>;
    const auto            *found = Seek(all, type, &Kind<VariantT>::Type);
    return found == nullptr || type.empty() ? all.back() : *found;
  }

  /**
   *  The records of a family in their order, each read with the room the ones before it found.
   */
  template <class VariantT>
  auto ReadOne(std::span<const File::Record> all, const Fields::Reading &with)
      -> std::vector<VariantT>
  {
    Fields::Scratch scratch;
    Fields::Reading reusing = with;
    reusing.Reuse           = &scratch;
    std::vector<VariantT> out;
    out.reserve(all.size());
    while (!all.empty())
    {
      out.push_back(Named<VariantT>(all.front()).Read(all, reusing));
    }
    return out;
  }

  /**
   *  The parts to read the records in, cut only before a record that none before it takes as its
   *  follower, so that no part begins inside a sequence.
   */
  template <class VariantT>
  auto Cut(std::span<const File::Record> all, int threads) -> std::vector<Range>
  {
    std::vector<Range> parts = Parted(all.size(), Parts(threads), Least);
    for (std::size_t at = 1; at < parts.size(); ++at)
    {
      std::size_t begin = parts.at(at).Begin;
      while (begin < all.size() && Trails<VariantT>(all[begin].Name))
      {
        ++begin;
      }
      parts.at(at - 1).End = begin;
      parts.at(at).Begin   = begin;
    }
    std::erase_if(parts, [](const Range &one) -> bool { return one.Begin >= one.End; });
    return parts;
  }

  /**
   *  The records of a family, read in parts on the threads given, each part with its room and its
   *  issues of its own, then joined in the file's order, the issues after the records as the one
   *  thread would have told them.
   */
  template <class VariantT>
  auto Read(std::span<const File::Record> all, const Fields::Reading &with) -> std::vector<VariantT>
  {
    const std::vector<Range> parts = Cut<VariantT>(all, with.Threads);
    if (parts.size() < 2)
    {
      return ReadOne<VariantT>(all, with);
    }
    std::vector<std::vector<VariantT>> pieces(parts.size());
    std::vector<std::vector<Issue>>    told(parts.size());
    Spread(parts.size(), with.Threads,
           [&](std::size_t at) -> void
           {
             Fields::Reading own = with;
             own.Issues          = &told.at(at);
             const Range &part   = parts.at(at);
             pieces.at(at) = ReadOne<VariantT>(all.subspan(part.Begin, part.End - part.Begin), own);
           });
    std::vector<VariantT> out;
    out.reserve(all.size());
    for (std::size_t at = 0; at < parts.size(); ++at)
    {
      std::ranges::move(pieces.at(at), std::back_inserter(out));
      for (Issue &issue : told.at(at))
      {
        with.Tell(std::move(issue));
      }
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
  auto WriteOne(Fields::Writing &to, std::span<const VariantT> all) -> std::expected<void, Failure>
  {
    return Chained(all, [&to](const VariantT &one) -> auto
                   { return Kinds<VariantT>.at(one.index()).Write(to, one); });
  }

  /**
   *  The records of a family written in parts on the threads given, each to text of its own, joined
   *  in their order; a failure's line is counted from the start of the file, past the parts before
   *  it.
   */
  template <class VariantT>
  auto Write(Fields::Writing &to, std::span<const VariantT> all) -> std::expected<void, Failure>
  {
    const std::vector<Range> parts = Parted(all.size(), Parts(to.Threads), Least);
    if (parts.size() < 2)
    {
      return WriteOne<VariantT>(to, all);
    }
    std::vector<std::string>                  texts(parts.size());
    std::vector<std::size_t>                  lines(parts.size());
    std::vector<std::expected<void, Failure>> done(parts.size());
    Spread(parts.size(), to.Threads,
           [&](std::size_t at) -> void
           {
             Tags::Writer    writer{texts.at(at), to.To->Formed()};
             Fields::Writing own = to;
             own.To              = &writer;
             const Range &part   = parts.at(at);
             done.at(at)  = WriteOne<VariantT>(own, all.subspan(part.Begin, part.End - part.Begin));
             lines.at(at) = writer.Lines();
           });
    for (std::size_t at = 0; at < parts.size(); ++at)
    {
      if (!done.at(at))
      {
        Failure failed  = done.at(at).error();
        failed.Line    += to.To->Lines();
        return std::unexpected{std::move(failed)};
      }
      to.To->Append(texts.at(at), lines.at(at));
    }
    return {};
  }
} // namespace DxfIO::Family

#endif /* DXFIO_FAMILY_HPP */
