#ifndef DXFIO_LINEWEIGHTS_HPP
#define DXFIO_LINEWEIGHTS_HPP

namespace DxfIO::Lineweights
{
  /**
   *  Lineweight -1 in group 370: the entity takes the lineweight of its layer.
   */
  inline constexpr int Bylayer = -1;

  /**
   *  Lineweight -2 in group 370: the entity takes the lineweight of the block insert it is drawn
   *  in.
   */
  inline constexpr int Byblock = -2;

  /**
   *  Lineweight -3 in group 370: the drawing's default lineweight, which $LWDEFAULT gives.
   */
  inline constexpr int Standard = -3;
} // namespace DxfIO::Lineweights

#endif /* DXFIO_LINEWEIGHTS_HPP */
