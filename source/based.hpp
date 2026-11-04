#ifndef DXFIO_BASED_HPP
#define DXFIO_BASED_HPP

#include "dxfio/base.hpp"

#include <concepts>
#include <type_traits>
#include <utility>

namespace DxfIO
{
template <class KindT>
  requires std::same_as<std::remove_cvref_t<decltype(std::declval<KindT &>().Base)>, DxfIO::Base>
auto Based(KindT &one) -> auto &
{
  return one.Base;
}

template <class KindT>
  requires std::same_as<std::remove_cvref_t<decltype(std::declval<KindT &>().Record.Base)>, DxfIO::Base>
auto Based(KindT &one) -> auto &
{
  return one.Record.Base;
}
} // namespace DxfIO

#endif /* DXFIO_BASED_HPP */
