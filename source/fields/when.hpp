#ifndef DXFIO_FIELDS_WHEN_HPP
#define DXFIO_FIELDS_WHEN_HPP

#include "fields/read.hpp"

namespace DxfIO::Fields
{
  template <class OwnerT, class FieldsT>
  struct When
  {
    bool (*Test)(const OwnerT &owner){nullptr};
    FieldsT Fields{};
  };

  template <class OwnerT, class FieldsT>
  void Take(const When<OwnerT, FieldsT> &shape, OwnerT &owner, Walk &in)
  {
    if (shape.Test(owner))
    {
      Read(owner, in, shape.Fields);
    }
  }

  template <class OwnerT, class FieldsT>
  auto Put(const When<OwnerT, FieldsT> &shape, const OwnerT &owner, Writing &to)
      -> std::expected<void, Failure>
  {
    if (!shape.Test(owner))
    {
      return {};
    }
    return Write(owner, to, shape.Fields);
  }
} // namespace DxfIO::Fields

#endif /* DXFIO_FIELDS_WHEN_HPP */
