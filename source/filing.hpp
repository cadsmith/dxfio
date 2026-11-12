#ifndef DXFIO_FILING_HPP
#define DXFIO_FILING_HPP

#include "based.hpp"
#include "fields/context.hpp"
#include "fields/owned.hpp"
#include "fields/read.hpp"
#include "fields/scratch.hpp"
#include "fields/text.hpp"
#include "fields/walk.hpp"
#include "file/record.hpp"
#include "head.hpp"
#include "shapes.hpp"
#include "tail.hpp"
#include "unknown/io.hpp"

#include "dxfio/failure.hpp"
#include "dxfio/issue.hpp"
#include "dxfio/unknown.hpp"

#include <cstddef>
#include <expected>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace DxfIO
{
  template <class KindT>
  inline constexpr int Self = Handled;

  template <class KindT>
  auto Filled(const File::Record &one, const Fields::Reading &with, bool &whole) -> KindT
  {
    KindT                            out{};
    const std::span<const Tags::Tag> body = one.Tags.subspan(1);
    const std::size_t                at   = Trailing(body);
    std::vector<Tags::Tag>           rest =
        DxfIO::Head(body.first(at), Heading{.Into = &Based(out), .Code = Self<KindT>}, with);
    {
      Fields::Walk in{rest, with};
      Fields::Read(out, in, Shapes<KindT>);
      Based(out).Extra = in.Kept();
      whole            = in.Whole();
    }
    Fields::Returned(with, std::move(rest));
    Tail(body.subspan(at), Based(out), with);
    return out;
  }

  template <class KindT>
  auto Taken(const File::Record &one, const Fields::Reading &with) -> KindT
  {
    bool whole = true;
    return Filled<KindT>(one, with, whole);
  }

  template <>
  inline auto Filled<Unknown>(const File::Record &one, const Fields::Reading &with,
                              bool & /*unused*/) -> Unknown
  {
    return Kept(one, with);
  }

  template <class KindT>
  auto Given(Fields::Writing &to, const KindT &one) -> std::expected<void, Failure>
  {
    const DxfIO::Base &base  = Based(one);
    const auto         extra = [&to, &base] -> auto
    {
      return to.Release == to.From ? Fields::Put(to, base.Extra) : std::expected<void, Failure>{};
    };
    return Fields::Word(to, 0, Type<KindT>)
        .and_then([&to, &base] -> auto { return DxfIO::Head(to, base, Self<KindT>); })
        .and_then([&to, &one] -> auto { return Fields::Write(one, to, Shapes<KindT>); })
        .and_then(extra)
        .and_then([&to, &base] -> auto { return Tail(to, base); });
  }

  template <class KindT>
  void Follow(KindT & /*unused*/, std::span<const File::Record> & /*unused*/,
              const Fields::Reading & /*unused*/)
  {
  }

  template <class KindT>
  auto Follow(Fields::Writing & /*unused*/, const KindT & /*unused*/)
      -> std::expected<void, Failure>
  {
    return {};
  }

  inline auto Given(Fields::Writing &to, const Unknown &one) -> std::expected<void, Failure>
  {
    return Put(to, one);
  }

  template <>
  inline constexpr std::string_view Type<Unknown>{};
} // namespace DxfIO

#endif /* DXFIO_FILING_HPP */
