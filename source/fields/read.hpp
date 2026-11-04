#ifndef DXFIO_FIELDS_READ_HPP
#define DXFIO_FIELDS_READ_HPP

#include "fields/context.hpp"
#include "fields/walk.hpp"

#include "dxfio/failure.hpp"

#include <expected>
#include <tuple>

namespace DxfIO::Fields
{
template <class OwnerT, class... ShapesT>
void Read(OwnerT &owner, Walk &in, const std::tuple<ShapesT...> &shapes)
{
  std::apply([&owner, &in](const auto &...shape) -> auto { (Take(shape, owner, in), ...); }, shapes);
}

template <class OwnerT, class... ShapesT>
auto Write(const OwnerT &owner, Writing &to, const std::tuple<ShapesT...> &shapes) -> std::expected<void, Failure>
{
  std::expected<void, Failure> done{};
  std::apply(
      [&owner, &to, &done](const auto &...shape) -> auto
      {
        static_cast<void>(
            ((done = done.and_then([&] -> std::expected<void, Failure> { return Put(shape, owner, to); })), ...));
      },
      shapes);
  return done;
}
} // namespace DxfIO::Fields

#endif /* DXFIO_FIELDS_READ_HPP */
