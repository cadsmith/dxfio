#ifndef DXFIO_VARIANTS_HPP
#define DXFIO_VARIANTS_HPP

#include "fields/context.hpp"
#include "fields/read.hpp"
#include "fields/walk.hpp"
#include "shapes.hpp"

#include "dxfio/failure.hpp"

#include <cstddef>
#include <expected>
#include <utility>
#include <variant>

namespace DxfIO
{
template <class VariantT, std::size_t At = 0>
void Become(VariantT &one, std::size_t index, Fields::Walk &in)
{
  if constexpr (At < std::variant_size_v<VariantT>)
  {
    if (At != index && index < std::variant_size_v<VariantT>)
    {
      Become<VariantT, At + 1>(one, index, in);
      return;
    }
    std::variant_alternative_t<At, VariantT> held{};
    Fields::Read(held, in, Shapes<decltype(held)>);
    one = std::move(held);
  }
}

template <class VariantT>
auto Spoken(const VariantT &one, Fields::Writing &to) -> std::expected<void, Failure>
{
  return std::visit([&to]<class KindT>(const KindT &held) -> auto { return Fields::Write(held, to, Shapes<KindT>); },
                    one);
}
} // namespace DxfIO

#endif /* DXFIO_VARIANTS_HPP */
