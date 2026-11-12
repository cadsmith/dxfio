#ifndef DXFIO_FIELDS_RUN_HPP
#define DXFIO_FIELDS_RUN_HPP

#include "chained.hpp"
#include "fields/read.hpp"

#include <expected>
#include <utility>
#include <vector>

namespace DxfIO::Fields
{
  template <class OwnerT, class ElementT, class FieldsT>
  struct Run
  {
    int                   Opener{};
    std::vector<ElementT> OwnerT::*Member{};
    FieldsT                        Fields{};
  };

  template <class OwnerT, class ElementT, class FieldsT>
  void Take(const Run<OwnerT, ElementT, FieldsT> &shape, OwnerT &owner, Walk &in)
  {
    while (in.Holds(shape.Opener))
    {
      ElementT one{};
      Read(one, in, shape.Fields);
      (owner.*shape.Member).push_back(std::move(one));
    }
  }

  template <class OwnerT, class ElementT, class FieldsT>
  auto Put(const Run<OwnerT, ElementT, FieldsT> &shape, const OwnerT &owner, Writing &to)
      -> std::expected<void, Failure>
  {
    return Chained(owner.*shape.Member, [&](const ElementT &element) -> auto
                   { return Write(element, to, shape.Fields); });
  }
} // namespace DxfIO::Fields

#endif /* DXFIO_FIELDS_RUN_HPP */
