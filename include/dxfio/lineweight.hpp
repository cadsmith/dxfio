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
    Mm000   =   0, /**< 0.00 mm. */
    Mm005   =   5, /**< 0.05 mm. */
    Mm009   =   9, /**< 0.09 mm. */
    Mm013   =  13, /**< 0.13 mm. */
    Mm015   =  15, /**< 0.15 mm. */
    Mm018   =  18, /**< 0.18 mm. */
    Mm020   =  20, /**< 0.20 mm. */
    Mm025   =  25, /**< 0.25 mm. */
    Mm030   =  30, /**< 0.30 mm. */
    Mm035   =  35, /**< 0.35 mm. */
    Mm040   =  40, /**< 0.40 mm. */
    Mm050   =  50, /**< 0.50 mm. */
    Mm053   =  53, /**< 0.53 mm. */
    Mm060   =  60, /**< 0.60 mm. */
    Mm070   =  70, /**< 0.70 mm. */
    Mm080   =  80, /**< 0.80 mm. */
    Mm090   =  90, /**< 0.90 mm. */
    Mm100   = 100, /**< 1.00 mm. */
    Mm106   = 106, /**< 1.06 mm. */
    Mm120   = 120, /**< 1.20 mm. */
    Mm140   = 140, /**< 1.40 mm. */
    Mm158   = 158, /**< 1.58 mm. */
    Mm200   = 200, /**< 2.00 mm. */
    Mm211   = 211, /**< 2.11 mm. */
  };

  /**
   *  Gives the width in millimeters; empty for ByLayer, ByBlock, Default and a width AutoCAD does
   *  not define.
   */
  [[nodiscard]] DXFIO_API auto MillimetersOf(Lineweight lineweight) -> std::optional<double>;
} // namespace DxfIO

#endif /* DXFIO_LINEWEIGHT_HPP */
