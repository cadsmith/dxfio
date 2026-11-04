#ifndef DXFIO_CORNERS_HPP
#define DXFIO_CORNERS_HPP

#include "dxfio/vector.hpp"

#include <array>

namespace DxfIO
{
/// The four corners of a 3DFACE, SOLID or TRACE, groups 10 to 13 in that order; a fourth left out is the third.
using Corners = std::array<Vector, 4>;
} // namespace DxfIO

#endif /* DXFIO_CORNERS_HPP */
