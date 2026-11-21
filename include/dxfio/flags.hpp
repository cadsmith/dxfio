#ifndef DXFIO_FLAGS_HPP
#define DXFIO_FLAGS_HPP

#include <type_traits>
#include <utility>

namespace DxfIO
{
  /**
   *  Tells whether a flags field has every bit of a named flag, such as `Has(polyline.Flags,
   *  PolylineFlags::Closed)`.
   */
  template <class FlagT>
    requires std::is_enum_v<FlagT>
  [[nodiscard]] constexpr auto Has(int flags, FlagT flag) -> bool
  {
    const int bits = static_cast<int>(std::to_underlying(flag));
    return (flags & bits) == bits;
  }
} // namespace DxfIO

#endif /* DXFIO_FLAGS_HPP */
