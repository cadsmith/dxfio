#ifndef DXFIO_FIELDS_FIELD_HPP
#define DXFIO_FIELDS_FIELD_HPP

#include "fields/context.hpp"
#include "fields/value.hpp"
#include "fields/walk.hpp"

#include "dxfio/api.hpp"
#include "dxfio/failure.hpp"
#include "dxfio/version.hpp"

#include <expected>
#include <optional>
#include <utility>

namespace DxfIO::Fields
{
  struct DXFIO_API None
  {
  };

  struct DXFIO_API Empty
  {
  };

  template <class OwnerT, class MemberT, class FallbackT = None>
  struct Field
  {
    int     Code{};
    MemberT OwnerT::*Member{};
    FallbackT        Fallback{};
    Version          Since{Version::R10};
  };

  template <class MemberT>
  void Fall(MemberT & /*unused*/, const None & /*unused*/)
  {
  }

  template <class MemberT>
  void Fall(std::optional<MemberT> &held, const Empty & /*unused*/)
  {
    held.reset();
  }

  template <class MemberT, class FallbackT>
  void Fall(MemberT &held, const FallbackT &to)
  {
    held = MemberT(to);
  }

  template <class MemberT>
  auto Fallen(const MemberT & /*unused*/, const None & /*unused*/) -> bool
  {
    return false;
  }

  template <class MemberT>
  auto Fallen(const std::optional<MemberT> &held, const Empty & /*unused*/) -> bool
  {
    return !held.has_value();
  }

  template <class MemberT, class FallbackT>
  auto Fallen(const MemberT &held, const FallbackT &to) -> bool
  {
    return held == to;
  }

  template <class OwnerT, class MemberT, class FallbackT>
  void Take(const Field<OwnerT, MemberT, FallbackT> &shape, OwnerT &owner, Walk &in)
  {
    if (in.With().Release < shape.Since)
    {
      return;
    }
    std::optional<MemberT> found = Value<MemberT>::Read(in, shape.Code);
    if (found)
    {
      owner.*shape.Member = std::move(*found);
      return;
    }
    Fall(owner.*shape.Member, shape.Fallback);
  }

  template <class OwnerT, class MemberT, class FallbackT>
  auto Put(const Field<OwnerT, MemberT, FallbackT> &shape, const OwnerT &owner, Writing &to)
      -> std::expected<void, Failure>
  {
    const MemberT &held = owner.*shape.Member;
    if (to.Release < shape.Since || Fallen(held, shape.Fallback))
    {
      return {};
    }
    return Value<MemberT>::Write(to, shape.Code, held);
  }
} // namespace DxfIO::Fields

#endif /* DXFIO_FIELDS_FIELD_HPP */
