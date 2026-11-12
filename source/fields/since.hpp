#ifndef DXFIO_FIELDS_SINCE_HPP
#define DXFIO_FIELDS_SINCE_HPP

#include "fields/read.hpp"

#include "dxfio/version.hpp"

namespace DxfIO::Fields
{
  template <class FieldsT>
  struct Since
  {
    Version Release{Version::R10};
    FieldsT Fields{};
  };

  template <class OwnerT, class FieldsT>
  void Take(const Since<FieldsT> &shape, OwnerT &owner, Walk &in)
  {
    if (in.With().Release >= shape.Release)
    {
      Read(owner, in, shape.Fields);
    }
  }

  template <class OwnerT, class FieldsT>
  auto Put(const Since<FieldsT> &shape, const OwnerT &owner, Writing &to)
      -> std::expected<void, Failure>
  {
    if (to.Release < shape.Release)
    {
      return {};
    }
    return Write(owner, to, shape.Fields);
  }
} // namespace DxfIO::Fields

#endif /* DXFIO_FIELDS_SINCE_HPP */
