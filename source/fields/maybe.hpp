#ifndef DXFIO_FIELDS_MAYBE_HPP
#define DXFIO_FIELDS_MAYBE_HPP

#include "fields/context.hpp"
#include "fields/read.hpp"
#include "fields/walk.hpp"

#include "dxfio/failure.hpp"

#include <expected>
#include <optional>
#include <utility>

namespace DxfIO::Fields
{
  template <class OwnerT, class HeldT, class FieldsT>
  struct Maybe
  {
    int                  Opener{};
    std::optional<HeldT> OwnerT::*Member{};
    FieldsT                       Fields{};
  };

  template <class OwnerT, class HeldT, class FieldsT>
  void Take(const Maybe<OwnerT, HeldT, FieldsT> &shape, OwnerT &owner, Walk &in)
  {
    in.Expect(shape.Opener);
    if (!in.Holds(shape.Opener))
    {
      return;
    }
    HeldT held{};
    Read(held, in, shape.Fields);
    owner.*shape.Member = std::move(held);
  }

  template <class OwnerT, class HeldT, class FieldsT>
  auto Put(const Maybe<OwnerT, HeldT, FieldsT> &shape, const OwnerT &owner, Writing &to)
      -> std::expected<void, Failure>
  {
    const std::optional<HeldT> &held = owner.*shape.Member;
    return held ? Write(*held, to, shape.Fields) : std::expected<void, Failure>{};
  }
} // namespace DxfIO::Fields

#endif /* DXFIO_FIELDS_MAYBE_HPP */
