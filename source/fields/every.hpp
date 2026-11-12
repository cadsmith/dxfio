#ifndef DXFIO_FIELDS_EVERY_HPP
#define DXFIO_FIELDS_EVERY_HPP

#include "chained.hpp"
#include "fields/elements.hpp"
#include "fields/read.hpp"

#include <utility>
#include <vector>

namespace DxfIO::Fields
{
  template <class OwnerT, class ElementT, class FieldsT>
  struct Every
  {
    int                   Opener{};
    std::vector<ElementT> OwnerT::*Member{};
    FieldsT                        Fields{};
  };

  template <class OwnerT, class ElementT, class FieldsT>
  void Take(const Every<OwnerT, ElementT, FieldsT> &shape, OwnerT &owner, Walk &in)
  {
    (owner.*shape.Member).clear();
    Elements(in, shape.Opener,
             [&] -> auto
             {
               ElementT element{};
               Read(element, in, shape.Fields);
               (owner.*shape.Member).push_back(std::move(element));
             });
  }

  template <class OwnerT, class ElementT, class FieldsT>
  auto Put(const Every<OwnerT, ElementT, FieldsT> &shape, const OwnerT &owner, Writing &to)
      -> std::expected<void, Failure>
  {
    return Chained(owner.*shape.Member, [&](const ElementT &element) -> auto
                   { return Write(element, to, shape.Fields); });
  }
} // namespace DxfIO::Fields

#endif /* DXFIO_FIELDS_EVERY_HPP */
