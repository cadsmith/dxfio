#ifndef DXFIO_ENTITIES_VIEWPORT_HPP
#define DXFIO_ENTITIES_VIEWPORT_HPP

#include "dxfio/api.hpp"
#include "dxfio/boxed.hpp"
#include "dxfio/entities/record.hpp"
#include "dxfio/handle.hpp"
#include "dxfio/tables/axes.hpp"
#include "dxfio/vector.hpp"

#include <optional>
#include <string>
#include <vector>

namespace DxfIO::Entities
{
  /**
   *  The circle zoom percent a new viewport takes, which sets how smoothly it draws circles and
   *  arcs.
   */
  inline constexpr int Roundness = 100;

  /**
   *  The status bits of a new viewport: only 32768, which AutoCAD always sets.
   */
  inline constexpr int Unflagged = 32768;

  /**
   *  How often a new viewport draws a major grid line, as $GRIDMAJOR and a VPORT's
   *  Tables::Gridmajor: every fifth line.
   */
  inline constexpr int Gridmajor = 5;

  /**
   *  The ambient light color of a new viewport, as an AutoCAD Color Index: a dark gray.
   */
  inline constexpr int Ambience = 250;

  /**
   *  The AcDbViewport subclass of a VIEWPORT: the window on the paper and the view of model space
   *  it shows.
   */
  struct DXFIO_API Port
  {
    /**
     *  The center of the window, in paper space coordinates. Groups 10, 20, 30.
     */
    Vector Center{};
    /**
     *  The width of the window, in paper space units. Group 40.
     */
    double Width{};
    /**
     *  The height of the window, in paper space units. Group 41.
     */
    double Height{};
    /**
     *  0 off, -1 on but not shown, or the viewport's place among those shown, 1 the active one.
     *  Group 68.
     */
    int Status{};
    /**
     *  The viewport's identifier, 1 for the one that shows the paper itself. Group 69.
     */
    int Id{};
    /**
     *  The center of the view, in display coordinates. Groups 12, 22, from R2000.
     */
    Vector View{};
    /**
     *  The base point of the snap. Groups 13, 23, from R2000.
     */
    Vector Base{};
    /**
     *  The spacing of the snap. Groups 14, 24, from R2000.
     */
    Vector Snap{};
    /**
     *  The spacing of the grid. Groups 15, 25, from R2000.
     */
    Vector Grid{};
    /**
     *  The direction the view looks from, toward its target, in world coordinates. Groups 16, 26,
     *  36, from R2000.
     */
    Vector Direction{Up};
    /**
     *  The point the view looks at, in world coordinates. Groups 17, 27, 37, from R2000.
     */
    Vector Target{};
    /**
     *  The lens length of a perspective view. Group 42, from R2000.
     */
    double Lens{};
    /**
     *  The offset of the front clipping plane from the target. Group 43, from R2000.
     */
    double Front{};
    /**
     *  The offset of the back clipping plane from the target. Group 44, from R2000.
     */
    double Back{};
    /**
     *  The height of the view, in model space units. Group 45, from R2000.
     */
    double Extent{-1};
    /**
     *  The snap angle. Group 50, from R2000.
     */
    double Angle{};
    /**
     *  The twist of the view. Group 51, from R2000.
     */
    double Twist{};
    /**
     *  The circle zoom percent. Group 72, from R2000.
     */
    int Zoom{Roundness};
    /**
     *  The layers frozen in this viewport alone. Group 341 before R2004, 331 from it.
     */
    std::vector<Handle> Frozen{};
    /**
     *  1 perspective, 2 front clip, 4 back clip, 8 UCS follows, 256 snap, 512 grid, 16384 scale
     *  locked, 65536 clipped by an entity, 131072 off, among others; 32768 always set. Group 90,
     *  from R2000.
     */
    int Flags{Unflagged};
    /**
     *  The entity that clips the viewport when it is not rectangular. Group 340, from R2000.
     */
    Handle Clip{};
    /**
     *  The name of the plot style sheet. Group 1, from R2000.
     */
    std::string Sheet{};
    /**
     *  The render mode: 0 2D wireframe, 1 3D wireframe, 2 hidden line, 3 flat shaded, 4 Gouraud
     *  shaded, 5 and 6 the same with wireframe. Group 281, from R2000.
     */
    int Render{};
    /**
     *  1 when the viewport keeps its own UCS, made current when it is active. Group 71, from R2000.
     */
    int Follows{1};
    /**
     *  1 when the UCS icon is drawn at the UCS origin. Group 74, from R2000.
     */
    int Icon{};
    /**
     *  The viewport's own UCS. Groups 110, 111, 112, 345, 346, 79 and 146, from R2000.
     */
    Tables::Axes Axes{};
    /**
     *  How the viewport is plotted: 0 as displayed, 1 wireframe, 2 hidden, 3 rendered. Group 170,
     *  from R2004.
     */
    int Shade{};
    /**
     *  How often a major grid line is drawn, in grid lines. Group 61, from R2007.
     */
    int Major{Gridmajor};
    /**
     *  The background drawn behind the view. Group 332, from R2007.
     */
    Handle Background{};
    /**
     *  The visual style or render preset the viewport is shade plotted with. Group 333, from R2007.
     */
    Handle Plot{};
    /**
     *  The visual style the view is drawn in. Group 348, from R2007.
     */
    Handle Style{};
    /**
     *  Whether the default lighting is on. Group 292, from R2007.
     */
    bool Lit{true};
    /**
     *  The default lighting: 1 one distant light, 2 two distant lights. Group 282, from R2007.
     */
    int Lighting{1};
    /**
     *  The brightness of the view. Group 141, from R2007.
     */
    double Brightness{};
    /**
     *  The contrast of the view. Group 142, from R2007.
     */
    double Contrast{};
    /**
     *  The ambient light color, as an AutoCAD Color Index. Group 63, from R2007.
     */
    int Ambient{Ambience};
    /**
     *  The ambient light's true color, as 0xRRGGBB. Group 421, from R2007.
     */
    std::optional<int> Rgb{};
    /**
     *  The color book of the ambient light, joined to the color's name by a $. Group 431, from
     *  R2007.
     */
    std::string Book{};
    /**
     *  The SUN that lights the viewport. Group 361, from R2007.
     */
    Handle Sun{};

    auto operator==(const Port &) const -> bool = default;
  };

  /**
   *  A VIEWPORT: a window on a layout's paper that shows a view of model space.
   */
  struct DXFIO_API Viewport
  {
    /**
     *  The layer, linetype, color and the rest every entity carries.
     */
    Entities::Record Record{};
    /**
     *  The window and the view, from the AcDbViewport subclass.
     */
    Boxed<Entities::Port> Port{};

    auto operator==(const Viewport &) const -> bool = default;
  };
} // namespace DxfIO::Entities

#endif /* DXFIO_ENTITIES_VIEWPORT_HPP */
