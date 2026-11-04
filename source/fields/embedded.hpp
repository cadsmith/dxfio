#ifndef DXFIO_FIELDS_EMBEDDED_HPP
#define DXFIO_FIELDS_EMBEDDED_HPP

#include "fields/context.hpp"
#include "fields/read.hpp"
#include "fields/walk.hpp"

#include "dxfio/api.hpp"
#include "dxfio/failure.hpp"

#include <expected>
#include <optional>
#include <utility>

namespace DxfIO::Fields
{
template <class OwnerT, class HeldT, class FieldsT>
struct Embedded
{
  std::optional<HeldT> OwnerT::*Member{};
  FieldsT Fields{};
};

[[nodiscard]] DXFIO_API auto Open(const Writing &to) -> std::expected<void, Failure>;

template <class OwnerT, class HeldT, class FieldsT>
void Take(const Embedded<OwnerT, HeldT, FieldsT> &shape, OwnerT &owner, Walk &in)
{
  if (!in.Embed())
  {
    return;
  }
  HeldT held{};
  Read(held, in, shape.Fields);
  owner.*shape.Member = std::move(held);
}

template <class OwnerT, class HeldT, class FieldsT>
auto Put(const Embedded<OwnerT, HeldT, FieldsT> &shape, const OwnerT &owner, Writing &to)
    -> std::expected<void, Failure>
{
  const std::optional<HeldT> &held = owner.*shape.Member;
  if (!held)
  {
    return {};
  }
  return Open(to).and_then([&] -> auto { return Write(*held, to, shape.Fields); });
}
} // namespace DxfIO::Fields

#endif /* DXFIO_FIELDS_EMBEDDED_HPP */
