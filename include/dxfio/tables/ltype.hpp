#ifndef DXFIO_TABLES_LTYPE_HPP
#define DXFIO_TABLES_LTYPE_HPP

#include "dxfio/api.hpp"
#include "dxfio/handle.hpp"
#include "dxfio/tables/record.hpp"

#include <string>
#include <vector>

namespace DxfIO::Tables
{
  /**
   *  The alignment every linetype has: 'A', the pattern fitted to the ends of the line.
   */
  inline constexpr int Aligned = 'A';

  /**
   *  One element of a linetype's pattern: a dash, a dot or a space, with the shape or text it may
   *  carry.
   */
  struct DXFIO_API Dash
  {
    /**
     *  The length: positive for a dash, 0 for a dot, negative for a space. Group 49.
     */
    double Length{};
    /**
     *  Bits: 1 when Rotation is absolute rather than along the line, 2 when Text is drawn, 4 when
     *  Shape is. Group 74, from R13.
     */
    int Flags{};
    /**
     *  The number of the shape drawn, in the shape file Style names; 0 for text. Group 75, from
     *  R13.
     */
    int Shape{};
    /**
     *  The text style or shape file the shape or text is drawn from. Group 340, from R13.
     */
    Handle Style{};
    /**
     *  The scale of the shape or text. Group 46, from R13.
     */
    double Scale{1};
    /**
     *  The rotation of the shape or text. Group 50, from R13.
     */
    double Rotation{};
    /**
     *  How far the shape or text is moved along the line. Group 44, from R13.
     */
    double Along{};
    /**
     *  How far the shape or text is moved across the line. Group 45, from R13.
     */
    double Across{};
    /**
     *  The text drawn, when Flags has 2. Group 9, from R13.
     */
    std::string Text{};

    auto operator==(const Dash &) const -> bool = default;
  };

  /**
   *  An LTYPE table record: the pattern of dashes, dots and spaces a line is drawn with.
   */
  struct DXFIO_API Ltype
  {
    /**
     *  The name, flags and handles every table record carries.
     */
    Tables::Record Record{};
    /**
     *  The text that tells what the pattern looks like, such as "__ __ __". Group 3.
     */
    std::string Description{};
    /**
     *  How the pattern fits the line; always 'A'. Group 72.
     */
    int Alignment{Aligned};
    /**
     *  The length of the whole pattern. Group 40.
     */
    double Length{};
    /**
     *  The elements of the pattern, in order; their count is group 73.
     */
    std::vector<Tables::Dash> Dashes{};

    auto operator==(const Ltype &) const -> bool = default;
  };
} // namespace DxfIO::Tables

#endif /* DXFIO_TABLES_LTYPE_HPP */
