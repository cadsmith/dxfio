#ifndef DXFIO_FIELDS_EACH_HPP
#define DXFIO_FIELDS_EACH_HPP

#include "chained.hpp"
#include "fields/read.hpp"

#include <cstddef>
#include <vector>

namespace DxfIO::Fields
{
  template <class OwnerT, class ElementT, class FieldsT>
  struct Each
  {
    int                   Opener{};
    std::vector<ElementT> OwnerT::*Member{};
    FieldsT                        Fields{};
  };

  template <class OwnerT, class ElementT, class FieldsT>
  void Take(const Each<OwnerT, ElementT, FieldsT> &shape, OwnerT &owner, Walk &in)
  {
    const Walk::Fence outer = in.Fenced();
    in.Rewind();
    std::vector<ElementT> &all    = owner.*shape.Member;
    std::size_t            filled = 0;
    for (; filled < all.size(); ++filled)
    {
      in.Narrow(shape.Opener, outer);
      if (in.Fenced().Begin >= outer.End)
      {
        break;
      }
      Read(all.at(filled), in, shape.Fields);
    }
    all.resize(filled);
    in.Restore(outer);
  }

  template <class OwnerT, class ElementT, class FieldsT>
  auto Put(const Each<OwnerT, ElementT, FieldsT> &shape, const OwnerT &owner, Writing &to)
      -> std::expected<void, Failure>
  {
    return Chained(owner.*shape.Member, [&](const ElementT &element) -> auto
                   { return Write(element, to, shape.Fields); });
  }
} // namespace DxfIO::Fields

#endif /* DXFIO_FIELDS_EACH_HPP */
