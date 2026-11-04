#ifndef DXFIO_FIELDS_JOINED_HPP
#define DXFIO_FIELDS_JOINED_HPP

#include "fields/context.hpp"
#include "fields/walk.hpp"

#include "dxfio/api.hpp"
#include "dxfio/failure.hpp"

#include <expected>
#include <string_view>

namespace DxfIO::Fields
{
struct DXFIO_API Joined
{
  std::string_view Marker;
};

DXFIO_API void Open(const Joined &shape, Walk &in);

[[nodiscard]] DXFIO_API auto Mark(const Joined &shape, const Writing &to) -> std::expected<void, Failure>;

template <class OwnerT>
void Take(const Joined &shape, OwnerT & /*unused*/, Walk &in)
{
  Open(shape, in);
}

template <class OwnerT>
auto Put(const Joined &shape, const OwnerT & /*unused*/, Writing &to) -> std::expected<void, Failure>
{
  return Mark(shape, to);
}
} // namespace DxfIO::Fields

#endif /* DXFIO_FIELDS_JOINED_HPP */
