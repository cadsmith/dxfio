#ifndef DXFIO_FIELDS_EITHER_HPP
#define DXFIO_FIELDS_EITHER_HPP

#include "fields/context.hpp"
#include "fields/subclass.hpp"
#include "fields/walk.hpp"

#include "dxfio/failure.hpp"

#include <array>
#include <cstddef>
#include <expected>
#include <span>
#include <string_view>

namespace DxfIO::Fields
{
  template <class OwnerT, std::size_t Size>
  struct Either
  {
    std::array<std::string_view, Size> Markers{};
    std::size_t (*Pick)(const OwnerT &owner){nullptr};
  };

  template <class OwnerT, std::size_t Size>
  void Take(const Either<OwnerT, Size> &shape, OwnerT & /*unused*/, Walk &in)
  {
    static_cast<void>(in.Subclass(std::span{shape.Markers}));
  }

  template <class OwnerT, std::size_t Size>
  auto Put(const Either<OwnerT, Size> &shape, const OwnerT &owner, Writing &to)
      -> std::expected<void, Failure>
  {
    return Mark(Subclass{.Marker = shape.Markers.at(shape.Pick(owner))}, to);
  }
} // namespace DxfIO::Fields

#endif /* DXFIO_FIELDS_EITHER_HPP */
