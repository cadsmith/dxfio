#ifndef DXFIO_FIELDS_PART_HPP
#define DXFIO_FIELDS_PART_HPP

#include "fields/read.hpp"

#include "dxfio/boxed.hpp"

namespace DxfIO::Fields
{
template <class MemberT>
auto Opened(MemberT &one) -> auto &
{
  if constexpr (requires { one.Valueless(); })
  {
    return *one;
  }
  else
  {
    return one;
  }
}

template <class OwnerT, class MemberT, class FieldsT>
struct Part
{
  MemberT OwnerT::*Member{};
  FieldsT Fields{};
};

template <class OwnerT, class MemberT, class FieldsT>
void Take(const Part<OwnerT, MemberT, FieldsT> &shape, OwnerT &owner, Walk &in)
{
  Read(Opened(owner.*shape.Member), in, shape.Fields);
}

template <class OwnerT, class MemberT, class FieldsT>
auto Put(const Part<OwnerT, MemberT, FieldsT> &shape, const OwnerT &owner, Writing &to) -> std::expected<void, Failure>
{
  return Write(Opened(owner.*shape.Member), to, shape.Fields);
}
} // namespace DxfIO::Fields

#endif /* DXFIO_FIELDS_PART_HPP */
