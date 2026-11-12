#ifndef DXFIO_FIELDS_SEQUENCE_HPP
#define DXFIO_FIELDS_SEQUENCE_HPP

#include "fields/context.hpp"
#include "fields/walk.hpp"

#include "dxfio/api.hpp"
#include "dxfio/failure.hpp"

#include <expected>
#include <string_view>

namespace DxfIO::Fields
{
  struct DXFIO_API Sequence
  {
    std::string_view Marker;
  };

  DXFIO_API void Open(const Sequence &shape, Walk &in);

  [[nodiscard]] DXFIO_API auto Mark(const Sequence &shape, const Writing &to)
      -> std::expected<void, Failure>;

  template <class OwnerT>
  void Take(const Sequence &shape, OwnerT & /*unused*/, Walk &in)
  {
    Open(shape, in);
  }

  template <class OwnerT>
  auto Put(const Sequence &shape, const OwnerT & /*unused*/, Writing &to)
      -> std::expected<void, Failure>
  {
    return Mark(shape, to);
  }
} // namespace DxfIO::Fields

#endif /* DXFIO_FIELDS_SEQUENCE_HPP */
