#ifndef DXFIO_FIELDS_CUSTOM_HPP
#define DXFIO_FIELDS_CUSTOM_HPP

#include "fields/context.hpp"
#include "fields/walk.hpp"

#include "dxfio/failure.hpp"

#include <expected>

namespace DxfIO::Fields
{
  template <class OwnerT>
  struct Custom
  {
    void (*Read)(OwnerT &owner, Walk &in){nullptr};
    std::expected<void, Failure> (*Write)(const OwnerT &owner, Writing &to){nullptr};
  };

  template <class OwnerT>
  void Take(const Custom<OwnerT> &shape, OwnerT &owner, Walk &in)
  {
    shape.Read(owner, in);
  }

  template <class OwnerT>
  auto Put(const Custom<OwnerT> &shape, const OwnerT &owner, Writing &to)
      -> std::expected<void, Failure>
  {
    return shape.Write(owner, to);
  }
} // namespace DxfIO::Fields

#endif /* DXFIO_FIELDS_CUSTOM_HPP */
