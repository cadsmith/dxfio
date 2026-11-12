#ifndef DXFIO_WIDTHS_HPP
#define DXFIO_WIDTHS_HPP

#include "dxfio/api.hpp"

namespace DxfIO
{
  /**
   *  The widths a polyline segment starts and ends with, groups 40 and 41 of the vertex it starts
   *  at.
   */
  struct DXFIO_API Widths
  {
    /**
     *  The width at the vertex, group 40.
     */
    double Start{};
    /**
     *  The width at the next vertex, group 41.
     */
    double End{};

    auto operator==(const Widths &) const -> bool = default;
  };
} // namespace DxfIO

#endif /* DXFIO_WIDTHS_HPP */
