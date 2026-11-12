#ifndef DXFIO_FIELDS_TALLY_HPP
#define DXFIO_FIELDS_TALLY_HPP

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
  struct Tally
  {
    int                   Code{};
    std::vector<ElementT> OwnerT::*Member{};
  };

  template <class OwnerT, class ElementT>
  void Take(const Tally<OwnerT, ElementT> &shape, OwnerT & /*unused*/, Walk &in)
  {
    static_cast<void>(Value<std::int64_t>::Read(in, shape.Code));
  }

  template <class OwnerT, class ElementT>
  auto Put(const Tally<OwnerT, ElementT> &shape, const OwnerT &owner, Writing &to)
      -> std::expected<void, Failure>
  {
    const auto size = static_cast<std::int64_t>((owner.*shape.Member).size());
    return Value<std::int64_t>::Write(to, shape.Code, size);
  }
} // namespace DxfIO::Fields

#endif /* DXFIO_FIELDS_TALLY_HPP */
