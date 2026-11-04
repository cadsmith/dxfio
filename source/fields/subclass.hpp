#ifndef DXFIO_FIELDS_SUBCLASS_HPP
#define DXFIO_FIELDS_SUBCLASS_HPP

#include "fields/context.hpp"
#include "fields/walk.hpp"

#include "dxfio/api.hpp"
#include "dxfio/failure.hpp"

#include <expected>
#include <string_view>

namespace DxfIO::Fields
{
struct DXFIO_API Subclass
{
  std::string_view Marker;
};

DXFIO_API void Open(const Subclass &shape, Walk &in);

[[nodiscard]] DXFIO_API auto Mark(const Subclass &shape, const Writing &to) -> std::expected<void, Failure>;

template <class OwnerT>
void Take(const Subclass &shape, OwnerT & /*unused*/, Walk &in)
{
  Open(shape, in);
}

template <class OwnerT>
auto Put(const Subclass &shape, const OwnerT & /*unused*/, Writing &to) -> std::expected<void, Failure>
{
  return Mark(shape, to);
}
} // namespace DxfIO::Fields

#endif /* DXFIO_FIELDS_SUBCLASS_HPP */
