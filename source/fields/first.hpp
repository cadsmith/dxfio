#ifndef DXFIO_FIELDS_FIRST_HPP
#define DXFIO_FIELDS_FIRST_HPP

#include "fields/read.hpp"
#include "fields/walk.hpp"

namespace DxfIO::Fields
{
  template <class FieldsT>
  struct First
  {
    int     Code{};
    FieldsT Fields{};
  };

  template <class OwnerT, class FieldsT>
  void Take(const First<FieldsT> &shape, OwnerT &owner, Walk &in)
  {
    const Walk::Fence outer = in.Fenced();
    in.Rewind();
    in.Narrow(shape.Code, outer);
    Read(owner, in, shape.Fields);
    in.Restore(outer);
  }

  template <class OwnerT, class FieldsT>
  auto Put(const First<FieldsT> &shape, const OwnerT &owner, Writing &to)
      -> std::expected<void, Failure>
  {
    return Write(owner, to, shape.Fields);
  }
} // namespace DxfIO::Fields

#endif /* DXFIO_FIELDS_FIRST_HPP */
