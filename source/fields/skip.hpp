#ifndef DXFIO_FIELDS_SKIP_HPP
#define DXFIO_FIELDS_SKIP_HPP

#include "fields/context.hpp"
#include "fields/walk.hpp"

#include "dxfio/api.hpp"
#include "dxfio/failure.hpp"

#include <expected>

namespace DxfIO::Fields
{
struct DXFIO_API Skip
{
  int Code{};
};

template <class OwnerT>
void Take(const Skip &shape, OwnerT & /*unused*/, Walk &in)
{
  while (in.Take(shape.Code) != nullptr)
  {
  }
}

template <class OwnerT>
auto Put(const Skip & /*unused*/, const OwnerT & /*unused*/, Writing & /*unused*/) -> std::expected<void, Failure>
{
  return {};
}
} // namespace DxfIO::Fields

#endif /* DXFIO_FIELDS_SKIP_HPP */
