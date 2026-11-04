#ifndef DXFIO_CHAINED_HPP
#define DXFIO_CHAINED_HPP

#include "dxfio/failure.hpp"

#include <concepts>
#include <expected>
#include <ranges>

namespace DxfIO
{
template <class StepT, class ElementT>
concept Writes = std::invocable<StepT &, ElementT> &&
                 std::same_as<std::invoke_result_t<StepT &, ElementT>, std::expected<void, Failure>>;

template <std::ranges::input_range RangeT, class StepT>
  requires Writes<StepT, std::ranges::range_reference_t<const RangeT>>
auto Chained(const RangeT &all, StepT step) -> std::expected<void, Failure>
{
  for (const auto &one : all)
  {
    std::expected<void, Failure> done = step(one);
    if (!done)
    {
      return done;
    }
  }
  return {};
}
} // namespace DxfIO

#endif /* DXFIO_CHAINED_HPP */
