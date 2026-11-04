#ifndef DXFIO_OBJECTS_TABLE_STYLE_HPP
#define DXFIO_OBJECTS_TABLE_STYLE_HPP

#include "dxfio/api.hpp"
#include "dxfio/base.hpp"
#include "dxfio/colors.hpp"
#include "dxfio/lineweights.hpp"

#include <string>
#include <vector>

namespace DxfIO::Objects::Table
{
/// A cell data type: general, any value.
inline constexpr int General = 512;

/// The text height of a new cell style.
inline constexpr double Height = 4.5;

/// The cell margins of a new table style.
inline constexpr double Margin = 1.5;

/// One border of a cell. Its groups depend on the border; each member gives them in the order top, inside
/// horizontal, bottom, left, inside vertical, right.
struct DXFIO_API Border
{
  /// The lineweight in hundredths of a millimeter, or -1 ByLayer, -2 ByBlock, -3 the default.
  /// Group 274 to 279 by border.
  int Lineweight{Lineweights::Byblock};
  /// Whether the border is drawn. Group 284 to 289 by border.
  bool Visible{true};
  /// The AutoCAD Color Index of the border. Group 64 to 69 by border.
  int Color{};

  auto operator==(const Border &) const -> bool = default;
};

/// How the cells of one kind of row are drawn: their text, fill and borders.
struct DXFIO_API Cell
{
  /// The name of the text style. Group 7.
  std::string Style;
  /// The height of the text. Group 140.
  double Height{Table::Height};
  /// Where text stands in the cell: 1 to 3 top left to top right, 4 to 6 middle, 7 to 9 bottom. Group 170.
  int Alignment{1};
  /// The AutoCAD Color Index of the text. Group 62.
  int Color{};
  /// The AutoCAD Color Index of the background. Group 63.
  int Fill{Colors::White};
  /// Whether the background is filled. Group 283.
  bool Filled{};
  /// The type of the cell's data, such as 512 for general. Group 90, from R2007.
  int Type{General};
  /// The unit of the cell's data. Group 91, from R2007.
  int Unit{};
  /// The format of the cell's data. Group 1, from R2007.
  std::string Format;
  /// The top border.
  Border Top{};
  /// The horizontal borders between rows.
  Border Middle{};
  /// The bottom border.
  Border Bottom{};
  /// The left border.
  Border Left{};
  /// The vertical borders between columns.
  Border Center{};
  /// The right border.
  Border Right{};

  auto operator==(const Cell &) const -> bool = default;
};
} // namespace DxfIO::Objects::Table

namespace DxfIO::Objects
{
/// A TABLESTYLE: how a table draws its title, header and data rows, named by its entry in the table style
/// dictionary.
struct DXFIO_API TableStyle
{
  /// The handle, owner, reactors and extended data.
  DxfIO::Base Base{};
  /// A version number, 0 as AutoCAD writes it. Group 280, the first, from R2010.
  int Revision{};
  /// The style's description; AutoCAD writes its name here. Group 3.
  std::string Description;
  /// Which way the table grows: 0 down, 1 up. Group 70.
  int Flow{};
  /// Flags, 0 in what AutoCAD writes. Group 71.
  int Flags{};
  /// The margin left and right of text in a cell. Group 40.
  double Horizontal{Table::Margin};
  /// The margin above and below text in a cell. Group 41.
  double Vertical{Table::Margin};
  /// Whether the title row is left out. Group 280.
  bool Untitled{};
  /// Whether the header row is left out. Group 281.
  bool Headless{};
  /// The cell styles of the data, title and header rows, in that order.
  std::vector<Table::Cell> Cells;

  auto operator==(const TableStyle &) const -> bool = default;
};
} // namespace DxfIO::Objects

#endif /* DXFIO_OBJECTS_TABLE_STYLE_HPP */
