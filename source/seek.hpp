#ifndef DXFIO_SEEK_HPP
#define DXFIO_SEEK_HPP

#include <algorithm>
#include <functional>
#include <ranges>

namespace DxfIO
{
template <std::ranges::contiguous_range RangeT, class ValueT, class ProjectionT = std::identity>
auto Seek(const RangeT &all, const ValueT &value, ProjectionT projection = {})
    -> const std::ranges::range_value_t<RangeT> *
{
  const auto found = std::ranges::find(all, value, projection);
  return found == std::ranges::end(all) ? nullptr : &*found;
}

template <std::ranges::contiguous_range RangeT, class PredicateT>
auto SeekIf(const RangeT &all, PredicateT predicate) -> const std::ranges::range_value_t<RangeT> *
{
  const auto found = std::ranges::find_if(all, predicate);
  return found == std::ranges::end(all) ? nullptr : &*found;
}
} // namespace DxfIO

#endif /* DXFIO_SEEK_HPP */
