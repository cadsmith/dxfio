#ifndef DXFIO_LINEWEIGHT_HPP
#define DXFIO_LINEWEIGHT_HPP

#include "dxfio/api.hpp"

#include <cstdint>
#include <optional>

namespace DxfIO
{
  /**
   *  A lineweight as group 370 gives it: one of AutoCAD's standard widths in hundredths of a
   *  millimeter, named by that width, or ByLayer, ByBlock or the default.
   */
  enum class Lineweight : std::int16_t
  {
    ByLayer = -1, /**< The layer's lineweight.                         */
    ByBlock = -2, /**< The lineweight of the insert drawing the block. */
    /**
     *  $LWDEFAULT.
     */
    Default =  -3,
    Mm000   =   0,
    Mm005   =   5,
    Mm009   =   9,
    Mm013   =  13,
    Mm015   =  15,
    Mm018   =  18,
    Mm020   =  20,
    Mm025   =  25,
    Mm030   =  30,
    Mm035   =  35,
    Mm040   =  40,
    Mm050   =  50,
    Mm053   =  53,
    Mm060   =  60,
    Mm070   =  70,
    Mm080   =  80,
    Mm090   =  90,
    Mm100   = 100,
    Mm106   = 106,
    Mm120   = 120,
    Mm140   = 140,
    Mm158   = 158,
    Mm200   = 200,
    Mm211   = 211,
  };

  /**
   *  Gives the width in millimeters; empty for ByLayer, ByBlock, Default and a width AutoCAD does
   *  not define.
   */
  [[nodiscard]] DXFIO_API auto MillimetersOf(Lineweight lineweight) -> std::optional<double>;
} // namespace DxfIO

#endif /* DXFIO_LINEWEIGHT_HPP */
