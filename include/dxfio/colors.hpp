#ifndef DXFIO_COLORS_HPP
#define DXFIO_COLORS_HPP

namespace DxfIO::Colors
{
/// Color 7 in group 62: drawn white on a dark background and black on a light one.
inline constexpr int White = 7;

/// Color 0 in group 62: the entity takes the color of the block insert it is drawn in.
inline constexpr int Byblock = 0;

/// Color 256 in group 62: the entity takes the color of its layer.
inline constexpr int Bylayer = 256;
} // namespace DxfIO::Colors

#endif /* DXFIO_COLORS_HPP */
