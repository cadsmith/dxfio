#ifndef DXFIO_TABLES_UCS_HPP
#define DXFIO_TABLES_UCS_HPP

#include "dxfio/api.hpp"
#include "dxfio/handle.hpp"
#include "dxfio/tables/record.hpp"
#include "dxfio/vector.hpp"

namespace DxfIO::Tables
{
  /**
   *  A UCS table record: a named user coordinate system.
   */
  struct DXFIO_API Ucs
  {
    /**
     *  The name, flags and handles every table record carries.
     */
    Tables::Record Record{};
    /**
     *  The origin, in world coordinates. Groups 10, 20, 30.
     */
    Vector Origin{};
    /**
     *  The direction of the X axis. Groups 11, 21, 31.
     */
    Vector X{.X = 1, .Y = 0, .Z = 0};
    /**
     *  The direction of the Y axis. Groups 12, 22, 32.
     */
    Vector Y{.X = 0, .Y = 1, .Z = 0};
    /**
     *  The orthographic view it stands for; AutoCAD writes 0. Group 79, from R2000.
     */
    int Ortho{};
    /**
     *  The elevation along the Z axis. Group 146, from R2000.
     */
    double Elevation{};
    /**
     *  The UCS an orthographic system is set from; none for the world. Group 346, from R2000.
     */
    Handle Basis{};

    auto operator==(const Ucs &) const -> bool = default;
  };
} // namespace DxfIO::Tables

#endif /* DXFIO_TABLES_UCS_HPP */
