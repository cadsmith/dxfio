#ifndef DXFIO_MATRIX_HPP
#define DXFIO_MATRIX_HPP

#include <array>

namespace DxfIO
{
  /**
   *  A 4 by 4 transform, such as a material map's, its 16 cells row by row in the order the file
   *  gives them as one group code repeated.
   */
  using Matrix = std::array<double, 16>;

  /**
   *  The transform that leaves everything where it is.
   */
  inline constexpr Matrix Identity{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
} // namespace DxfIO

#endif /* DXFIO_MATRIX_HPP */
