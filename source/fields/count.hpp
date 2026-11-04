#ifndef DXFIO_FIELDS_COUNT_HPP
#define DXFIO_FIELDS_COUNT_HPP

#include "fields/context.hpp"
#include "fields/numbers.hpp"
#include "fields/walk.hpp"

#include "dxfio/failure.hpp"

#include <cstdint>
#include <expected>
#include <vector>

namespace DxfIO::Fields
{
template <class OwnerT, class ElementT>
struct Count
{
  int Code{};
  std::vector<ElementT> OwnerT::*Member{};
};

template <class OwnerT, class ElementT>
void Take(const Count<OwnerT, ElementT> &shape, OwnerT &owner, Walk &in)
{
  const std::int64_t wanted = Value<std::int64_t>::Read(in, shape.Code).value_or(0);
  (owner.*shape.Member).assign(in.Claim(wanted), ElementT{});
}

template <class OwnerT, class ElementT>
auto Put(const Count<OwnerT, ElementT> &shape, const OwnerT &owner, Writing &to) -> std::expected<void, Failure>
{
  const auto size = static_cast<std::int64_t>((owner.*shape.Member).size());
  return Value<std::int64_t>::Write(to, shape.Code, size);
}
} // namespace DxfIO::Fields

#endif /* DXFIO_FIELDS_COUNT_HPP */
