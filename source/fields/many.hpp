#ifndef DXFIO_FIELDS_MANY_HPP
#define DXFIO_FIELDS_MANY_HPP

#include "chained.hpp"
#include "fields/context.hpp"
#include "fields/elements.hpp"
#include "fields/value.hpp"
#include "fields/walk.hpp"

#include "dxfio/failure.hpp"

#include <expected>
#include <optional>
#include <utility>
#include <vector>

namespace DxfIO::Fields
{
  template <class OwnerT, class ElementT>
  struct Many
  {
    int                   Code{};
    std::vector<ElementT> OwnerT::*Member{};
  };

  template <class OwnerT, class ElementT>
  void Take(const Many<OwnerT, ElementT> &shape, OwnerT &owner, Walk &in)
  {
    Elements(in, shape.Code,
             [&] -> auto
             {
               if (std::optional<ElementT> one = Value<ElementT>::Read(in, shape.Code))
               {
                 (owner.*shape.Member).push_back(std::move(*one));
               }
             });
  }

  template <class OwnerT, class ElementT>
  auto Put(const Many<OwnerT, ElementT> &shape, const OwnerT &owner, Writing &to)
      -> std::expected<void, Failure>
  {
    return Chained(owner.*shape.Member, [&](const ElementT &one) -> auto
                   { return Value<ElementT>::Write(to, shape.Code, one); });
  }
} // namespace DxfIO::Fields

#endif /* DXFIO_FIELDS_MANY_HPP */
