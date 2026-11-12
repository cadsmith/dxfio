#ifndef DXFIO_ENTITIES_SHAPES_TRAIL_HPP
#define DXFIO_ENTITIES_SHAPES_TRAIL_HPP

#include "fields/context.hpp"
#include "fields/custom.hpp"
#include "fields/owned.hpp"
#include "fields/walk.hpp"

#include "dxfio/failure.hpp"
#include "dxfio/group.hpp"

#include <expected>
#include <vector>

namespace DxfIO::Entities
{
  template <class OwnerT, std::vector<Group> OwnerT::*MemberV>
  void Trailing(OwnerT &one, Fields::Walk &in)
  {
    one.*MemberV = in.Rest();
    in.Spend();
  }

  template <class OwnerT, std::vector<Group> OwnerT::*MemberV>
  auto Trail(const OwnerT &one, Fields::Writing &to) -> std::expected<void, Failure>
  {
    return Fields::Put(to, one.*MemberV);
  }

  template <class OwnerT, std::vector<Group> OwnerT::*MemberV>
  inline constexpr Fields::Custom<OwnerT> Trailed{
      .Read  = Trailing<OwnerT, MemberV>,
      .Write = Trail<OwnerT, MemberV>,
  };
} // namespace DxfIO::Entities

#endif /* DXFIO_ENTITIES_SHAPES_TRAIL_HPP */
