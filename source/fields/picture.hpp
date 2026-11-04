#ifndef DXFIO_FIELDS_PICTURE_HPP
#define DXFIO_FIELDS_PICTURE_HPP

#include "fields/context.hpp"
#include "fields/walk.hpp"

#include "dxfio/api.hpp"
#include "dxfio/failure.hpp"

#include <expected>
#include <string>

namespace DxfIO::Fields
{
template <class OwnerT>
struct Picture
{
  std::string OwnerT::*Member{};
};

[[nodiscard]] DXFIO_API auto Pictured(Walk &in) -> std::string;

[[nodiscard]] DXFIO_API auto Show(Writing &to, const std::string &held) -> std::expected<void, Failure>;

template <class OwnerT>
void Take(const Picture<OwnerT> &shape, OwnerT &owner, Walk &in)
{
  owner.*shape.Member = Pictured(in);
}

template <class OwnerT>
auto Put(const Picture<OwnerT> &shape, const OwnerT &owner, Writing &to) -> std::expected<void, Failure>
{
  return Show(to, owner.*shape.Member);
}
} // namespace DxfIO::Fields

#endif /* DXFIO_FIELDS_PICTURE_HPP */
