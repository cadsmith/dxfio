#ifndef DXFIO_TABLES_AXES_HPP
#define DXFIO_TABLES_AXES_HPP

#include "dxfio/api.hpp"
#include "dxfio/handle.hpp"
#include "dxfio/vector.hpp"

namespace DxfIO::Tables
{
  /**
   *  The user coordinate system a view, a viewport or a layout keeps with it.
   */
  struct DXFIO_API Axes
  {
    /**
     *  The origin. Groups 110, 120, 130, or 13, 23, 33 in a layout.
     */
    Vector Origin{};
    /**
     *  The direction of the X axis. Groups 111, 121, 131, or 16, 26, 36 in a layout.
     */
    Vector X{.X = 1, .Y = 0, .Z = 0};
    /**
     *  The direction of the Y axis. Groups 112, 122, 132, or 17, 27, 37 in a layout.
     */
    Vector Y{.X = 0, .Y = 1, .Z = 0};
    /**
     *  The orthographic view it stands for: 0 none, 1 top, 2 bottom, 3 front, 4 back, 5 left, 6
     *  right. Group 79, or 76 in a layout.
     */
    int Ortho{};
    /**
     *  The elevation along the Z axis. Group 146.
     */
    double Elevation{};
    /**
     *  The UCS record, when the system is a named one. Group 345.
     */
    Handle Named{};
    /**
     *  The UCS record an orthographic system is set from; none for the world. Group 346.
     */
    Handle Basis{};

    auto operator==(const Axes &) const -> bool = default;
  };
} // namespace DxfIO::Tables

#endif /* DXFIO_TABLES_AXES_HPP */
