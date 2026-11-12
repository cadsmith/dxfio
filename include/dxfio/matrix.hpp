#ifndef DXFIO_MATRIX_HPP
#define DXFIO_MATRIX_HPP

#include "dxfio/api.hpp"

#include <array>
#include <cstddef>

namespace DxfIO
{
  /**
   *  The number of rows, and of columns, of a Matrix.
   */
  inline constexpr std::size_t Side = 4;

  /**
   *  A 4 by 4 transform, such as a material map's, whose 16 cells DXF gives as one group code
   *  repeated.
   */
  struct DXFIO_API Matrix
  {
    /**
     *  The 16 cells, in the order the file gives them; the identity until set.
     */
    std::array<double, Side * Side> Cells{
        1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1,
    };

    auto operator==(const Matrix &) const -> bool = default;
  };

  /**
   *  The transform that leaves everything where it is.
   */
  inline constexpr Matrix Identity{};
} // namespace DxfIO

#endif /* DXFIO_MATRIX_HPP */
