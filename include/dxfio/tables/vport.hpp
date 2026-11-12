#ifndef DXFIO_TABLES_VPORT_HPP
#define DXFIO_TABLES_VPORT_HPP

#include "dxfio/api.hpp"
#include "dxfio/handle.hpp"
#include "dxfio/tables/axes.hpp"
#include "dxfio/tables/record.hpp"
#include "dxfio/tables/view.hpp"
#include "dxfio/vector.hpp"

#include <optional>

namespace DxfIO::Tables
{
  /**
   *  The snap spacing of a new viewport, as $SNAPUNIT.
   */
  inline constexpr double Snapunit = 10;

  /**
   *  The grid spacing of a new viewport, as $GRIDUNIT.
   */
  inline constexpr double Gridunit = 10;

  /**
   *  The circle zoom percent of a new viewport, as $VIEWRES.
   */
  inline constexpr int Viewres = 1000;

  /**
   *  The grid lines between major ones in a new viewport.
   */
  inline constexpr int Gridmajor = 5;

  /**
   *  The color of a new viewport's ambient light: color index 250, a dark gray.
   */
  inline constexpr int Ambience = 250;

  /**
   *  A VPORT table record: a tiled viewport of model space. The tiled viewports on screen together
   *  share the name "*Active".
   */
  struct DXFIO_API Vport
  {
    /**
     *  The name, flags and handles every table record carries.
     */
    Tables::Record Record{};
    /**
     *  The lower left corner on screen, from 0 to 1 across and up. Groups 10, 20.
     */
    Vector Lower{};
    /**
     *  The upper right corner on screen, from 0 to 1 across and up. Groups 11, 21.
     */
    Vector Upper{.X = 1, .Y = 1, .Z = 0};
    /**
     *  The center of the view, in display coordinates. Groups 12, 22.
     */
    Vector Center{};
    /**
     *  The snap base point, in display coordinates. Groups 13, 23.
     */
    Vector Base{};
    /**
     *  The snap spacing along X and Y. Groups 14, 24.
     */
    Vector Snap{.X = Snapunit, .Y = Snapunit, .Z = 0};
    /**
     *  The grid spacing along X and Y. Groups 15, 25.
     */
    Vector Grid{.X = Gridunit, .Y = Gridunit, .Z = 0};
    /**
     *  The direction the view looks from, toward the target, in world coordinates. Groups 16, 26,
     *  36.
     */
    Vector Direction{.X = 0, .Y = 0, .Z = 1};
    /**
     *  The point the view looks at, in world coordinates. Groups 17, 27, 37.
     */
    Vector Target{};
    /**
     *  How tall the view is, in display coordinates. Group 40.
     */
    double Height{1};
    /**
     *  The width of the viewport over its height. Group 41.
     */
    double Aspect{1};
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
     *  The snap angle, in degrees. Group 50.
     */
    double Rotation{};
    /**
     *  The rotation of the view about its direction, in degrees. Group 51.
     */
    double Twist{};
    /**
     *  Bits, as $VIEWMODE: 1 perspective, 2 front clipping, 4 back clipping, 8 UCS follow, 16 front
     *  clip not at the eye. Group 71.
     */
    int Mode{};
    /**
     *  The circle zoom percent, as $VIEWRES: how smooth arcs and circles are drawn. Group 72.
     */
    int Zoom{Viewres};
    /**
     *  Whether fast zoom is on. Group 73.
     */
    bool Fast{true};
    /**
     *  The UCS icon, as $UCSICON: 1 shown, 2 at the origin. Group 74.
     */
    int Icon{3};
    /**
     *  Whether snap is on. Group 75.
     */
    bool Snapped{};
    /**
     *  Whether the grid is shown. Group 76.
     */
    bool Gridded{true};
    /**
     *  Whether snap is isometric rather than square. Group 77.
     */
    bool Isometric{};
    /**
     *  The isometric plane: 0 left, 1 top, 2 right. Group 78.
     */
    int Isoplane{};
    /**
     *  How the view is shaded: 0 2D optimized, 1 wireframe, 2 hidden line, 3 flat, 4 Gouraud, 5 and
     *  6 those two with wireframe. Group 281, from R2000.
     */
    int Render{};
    /**
     *  Whether the viewport keeps a UCS of its own, made current when the viewport is. Group 65,
     *  from R2000.
     */
    bool Ucsvp{true};
    /**
     *  The UCS of the viewport; from R2000.
     */
    Tables::Axes Axes{};
    /**
     *  The visual style. Group 348, from R2007.
     */
    Handle Style{};
    /**
     *  How the grid is shown, as $GRIDDISPLAY bits. Group 60, from R2007.
     */
    int Display{3};
    /**
     *  The grid lines between major ones, as $GRIDMAJOR. Group 61, from R2007.
     */
    int Major{Gridmajor};
    /**
     *  Whether the default lighting is on. Group 292, from R2007.
     */
    bool Lit{true};
    /**
     *  The default lighting: 0 one distant light, 1 two. Group 282, from R2007.
     */
    int Lighting{1};
    /**
     *  How bright the viewport's lighting is. Group 141, from R2007.
     */
    double Brightness{};
    /**
     *  How much the viewport's lighting contrasts. Group 142, from R2007.
     */
    double Contrast{};
    /**
     *  The AutoCAD Color Index of the ambient light. Group 63, from R2007.
     */
    int Ambient{Ambience};
    /**
     *  The ambient light as a true color 0xRRGGBB, when it has one. Group 421, from R2007.
     */
    std::optional<int> Rgb{};
    /**
     *  The background object. Group 332, from R2007.
     */
    Handle Background{};
    /**
     *  The shade plot object. Group 333, from R2007.
     */
    Handle Shadow{};

    auto operator==(const Vport &) const -> bool = default;
  };
} // namespace DxfIO::Tables

#endif /* DXFIO_TABLES_VPORT_HPP */
