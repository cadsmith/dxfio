#ifndef DXFIO_OBJECTS_MLINE_STYLE_HPP
#define DXFIO_OBJECTS_MLINE_STYLE_HPP

#include "dxfio/api.hpp"
#include "dxfio/base.hpp"
#include "dxfio/colors.hpp"

#include <string>
#include <vector>

namespace DxfIO::Objects::Mline
{
  /**
   *  The start and end angle of a new multiline style, in degrees: square to the line.
   */
  inline constexpr double Square = 90;

  /**
   *  One of the parallel lines a multiline style draws.
   */
  struct DXFIO_API Element
  {
    /**
     *  How far the line runs from the multiline's center line. Group 49.
     */
    double Offset{};
    /**
     *  The AutoCAD Color Index of the line: 1 to 255, 0 for ByBlock, 256 for ByLayer. Group 62.
     */
    int Color{Colors::Bylayer};
    /**
     *  The name of the line's linetype. Group 6.
     */
    std::string Linetype{"BYLAYER"};

    auto operator==(const Element &) const -> bool = default;
  };
} // namespace DxfIO::Objects::Mline

namespace DxfIO::Objects
{
  /**
   *  An MLINESTYLE: the parallel lines, fill and end caps a multiline is drawn with.
   */
  struct DXFIO_API MLineStyle
  {
    /**
     *  The handle, owner, reactors and extended data.
     */
    DxfIO::Base Base{};
    /**
     *  The name of the style, such as "Standard". Group 2.
     */
    std::string Name{};
    /**
     *  Bits: 1 filled, 2 miters shown, 16, 32 and 64 a line, inner arc or outer arc capping the
     *  start, 256, 512 and 1024 the same at the end. Group 70.
     */
    int Flags{};
    /**
     *  The style's description. Group 3.
     */
    std::string Description{};
    /**
     *  The AutoCAD Color Index of the fill. Group 62.
     */
    int Fill{Colors::Bylayer};
    /**
     *  The angle of the start cap, in degrees. Group 51.
     */
    double Start{Mline::Square};
    /**
     *  The angle of the end cap, in degrees. Group 52.
     */
    double End{Mline::Square};
    /**
     *  The lines, in order; their count is group 71.
     */
    std::vector<Mline::Element> Elements{};

    auto operator==(const MLineStyle &) const -> bool = default;
  };
} // namespace DxfIO::Objects

#endif /* DXFIO_OBJECTS_MLINE_STYLE_HPP */
