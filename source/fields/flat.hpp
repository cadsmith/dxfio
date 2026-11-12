#ifndef DXFIO_FIELDS_FLAT_HPP
#define DXFIO_FIELDS_FLAT_HPP

#include "fields/context.hpp"
#include "fields/numbers.hpp"
#include "fields/points.hpp"
#include "fields/walk.hpp"

#include "dxfio/failure.hpp"
#include "dxfio/vector.hpp"

#include <expected>
#include <optional>

namespace DxfIO::Fields
{
  template <class OwnerT>
  struct Flat
  {
    int    Code{};
    Vector OwnerT::*Member{};
  };

  template <class OwnerT>
  void Take(const Flat<OwnerT> &shape, OwnerT &owner, Walk &in)
  {
    Vector &held = owner.*shape.Member;
    held.X       = Value<double>::Read(in, shape.Code).value_or(held.X);
    held.Y       = Value<double>::Read(in, shape.Code + Axis).value_or(held.Y);
    held.Z       = Value<double>::Read(in, shape.Code + (2 * Axis)).value_or(held.Z);
  }

  template <class OwnerT>
  auto Put(const Flat<OwnerT> &shape, const OwnerT &owner, Writing &to)
      -> std::expected<void, Failure>
  {
    const Vector &held = owner.*shape.Member;
    return Value<double>::Write(to, shape.Code, held.X)
        .and_then([&] -> auto { return Value<double>::Write(to, shape.Code + Axis, held.Y); })
        .and_then(
            [&] -> auto
            {
              return held.Z == 0 ? std::expected<void, Failure>{}
                                 : Value<double>::Write(to, shape.Code + (2 * Axis), held.Z);
            });
  }
} // namespace DxfIO::Fields

#endif /* DXFIO_FIELDS_FLAT_HPP */
