#ifndef DXFIO_TABLES_VIEW_HPP
#define DXFIO_TABLES_VIEW_HPP

#include "dxfio/api.hpp"
#include "dxfio/handle.hpp"
#include "dxfio/tables/axes.hpp"
#include "dxfio/tables/record.hpp"
#include "dxfio/vector.hpp"

namespace DxfIO::Tables
{
  /**
   *  The lens length, in millimeters, of a new view or viewport.
   */
  inline constexpr double Lens = 50;

  /**
   *  A VIEW table record: a named view of the drawing, with the camera it is seen through.
   */
  struct DXFIO_API View
  {
    /**
     *  The name, flags and handles every table record carries.
     */
    Tables::Record Record{};
    /**
     *  How tall the view is, in display coordinates. Group 40.
     */
    double Height{1};
    /**
     *  The center of the view, in display coordinates. Groups 10, 20.
     */
    Vector Center{};
    /**
     *  How wide the view is, in display coordinates. Group 41.
     */
    double Width{1};
    /**
     *  The direction the view looks from, toward the target, in world coordinates. Groups 11, 21,
     *  31.
     */
    Vector Direction{.X = 0, .Y = 0, .Z = 1};
    /**
     *  The point the view looks at, in world coordinates. Groups 12, 22, 32.
     */
    Vector Target{};
    /**
     *  The lens length, in millimeters, of a perspective view. Group 42.
     */
    double Focal{Lens};
    /**
     *  The front clipping plane, as an offset from the target. Group 43.
     */
    double Front{};
    /**
     *  The back clipping plane, as an offset from the target. Group 44.
     */
    double Back{};
    /**
     *  The rotation of the view about its direction, in degrees. Group 50.
     */
    double Twist{};
    /**
     *  Bits, as $VIEWMODE: 1 perspective, 2 front clipping, 4 back clipping, 8 UCS follow, 16 front
     *  clip not at the eye. Group 71.
     */
    int Mode{};
    /**
     *  How the view is shaded: 0 2D optimized, 1 wireframe, 2 hidden line, 3 flat, 4 Gouraud, 5 and
     *  6 those two with wireframe. Group 281, from R2000.
     */
    int Render{};
    /**
     *  Whether the view keeps a UCS of its own in Axes. Group 72, from R2000.
     */
    bool Associated{};
    /**
     *  The UCS of the view, written only when Associated is set; from R2000.
     */
    Tables::Axes Axes{};
    /**
     *  Whether the camera is plotted. Group 73, from R2007.
     */
    bool Plottable{};
    /**
     *  The background object. Group 332, from R2007.
     */
    Handle Background{};
    /**
     *  The live section object. Group 334, from R2007.
     */
    Handle Section{};
    /**
     *  The visual style. Group 348, from R2007.
     */
    Handle Style{};

    auto operator==(const View &) const -> bool = default;
  };
} // namespace DxfIO::Tables

#endif /* DXFIO_TABLES_VIEW_HPP */
