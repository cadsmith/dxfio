#ifndef DXFIO_ENTITIES_SHAPES_IDENTIFIER_HPP
#define DXFIO_ENTITIES_SHAPES_IDENTIFIER_HPP

#include "fields/custom.hpp"
#include "fields/field.hpp"
#include "fields/numbers.hpp"

#include "dxfio/failure.hpp"
#include "dxfio/version.hpp"

#include <expected>

namespace DxfIO::Entities
{
template <class OwnerT>
inline constexpr Fields::Field<OwnerT, int, int> Identity{
    .Code = 91,
    .Member = &OwnerT::Id,
    .Fallback = 0,
    .Since = Version::R13,
};

template <class OwnerT>
void Identify(OwnerT &one, Fields::Walk &in)
{
  Fields::Take(Identity<OwnerT>, one, in);
}

template <class OwnerT>
auto Identifying(const OwnerT &one, Fields::Writing &to) -> std::expected<void, Failure>
{
  if (to.Release < Version::R2010 && to.Release != to.From)
  {
    return {};
  }
  return Fields::Put(Identity<OwnerT>, one, to);
}

template <class OwnerT>
inline constexpr Fields::Custom<OwnerT> Identifier{
    .Read = Identify<OwnerT>,
    .Write = Identifying<OwnerT>,
};
} // namespace DxfIO::Entities

#endif /* DXFIO_ENTITIES_SHAPES_IDENTIFIER_HPP */
