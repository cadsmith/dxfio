#ifndef DXFIO_COLORS_HPP
#define DXFIO_COLORS_HPP

namespace DxfIO::Colors
{
  /**
   *  Color 7 in group 62: drawn white on a dark background and black on a light one.
   */
  inline constexpr int White = 7;

  /**
   *  Color 0 in group 62: the entity takes the color of the block insert it is drawn in.
   */
  inline constexpr int ByBlock = 0;

  /**
   *  Color 256 in group 62: the entity takes the color of its layer.
   */
  inline constexpr int ByLayer = 256;

  /**
   *  ByLayer as AutoCAD packs a color into one integer: the color method 0xC0 in the top byte and
   *  nothing below it.
   */
  inline constexpr int PackedByLayer = -1073741824;

  /**
   *  ByBlock as AutoCAD packs a color into one integer: the color method 0xC1 in the top byte and
   *  nothing below it.
   */
  inline constexpr int PackedByBlock = -1056964608;

  /**
   *  Black as AutoCAD packs a true color into one integer: the color method 0xC2 in the top byte
   *  and 0x000000 below it, what a new material's colors hold.
   */
  inline constexpr int PackedBlack = -1040187392;
} // namespace DxfIO::Colors

#endif /* DXFIO_COLORS_HPP */
