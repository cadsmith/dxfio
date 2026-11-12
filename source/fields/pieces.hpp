#ifndef DXFIO_FIELDS_PIECES_HPP
#define DXFIO_FIELDS_PIECES_HPP

#include "fields/context.hpp"
#include "fields/walk.hpp"

#include "dxfio/api.hpp"
#include "dxfio/failure.hpp"

#include <expected>
#include <string>

namespace DxfIO::Fields
{
  template <class OwnerT>
  struct Pieces
  {
    std::string OwnerT::*Member{};
  };

  [[nodiscard]] DXFIO_API auto Gathered(Walk &in) -> std::string;

  [[nodiscard]] DXFIO_API auto Split(Writing &to, const std::string &text)
      -> std::expected<void, Failure>;

  template <class OwnerT>
  void Take(const Pieces<OwnerT> &shape, OwnerT &owner, Walk &in)
  {
    owner.*shape.Member = Gathered(in);
  }

  template <class OwnerT>
  auto Put(const Pieces<OwnerT> &shape, const OwnerT &owner, Writing &to)
      -> std::expected<void, Failure>
  {
    return Split(to, owner.*shape.Member);
  }
} // namespace DxfIO::Fields

#endif /* DXFIO_FIELDS_PIECES_HPP */
