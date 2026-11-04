#ifndef DXFIO_ENTITIES_MTEXT_HPP
#define DXFIO_ENTITIES_MTEXT_HPP

#include "dxfio/api.hpp"
#include "dxfio/boxed.hpp"
#include "dxfio/entities/record.hpp"
#include "dxfio/symbol.hpp"
#include "dxfio/vector.hpp"

#include <optional>
#include <string>
#include <vector>

namespace DxfIO::Entities
{
/// The columns of an MTEXT, which R2018 writes as an embedded object.
struct DXFIO_API Columns
{
  /// The attachment point, as group 71 of the MTEXT gives it. Group 70.
  int Attachment{1};
  /// The direction of the text, in world coordinates. Groups 10, 20, 30.
  Vector Direction{.X = 1};
  /// The insertion point, in world coordinates. Groups 11, 21, 31.
  Vector Position{};
  /// The width of the text's frame. Group 40.
  double Width{};
  /// The height of the text's frame. Group 41.
  double Height{};
  /// The width the text fills. Group 42.
  double Extent{};
  /// The height the text fills. Group 43.
  double Depth{};
  /// 0 no columns, 1 static, 2 dynamic. Group 71.
  int Kind{};
  /// The number of columns. Group 72.
  int Count{1};
  /// The width of a column. Group 44.
  double Breadth{1};
  /// The gap between columns. Group 45.
  double Gutter{};
  /// Whether the height of the columns follows the text. Group 73.
  bool Automatic{};
  /// Whether the text flows through the columns in reverse. Group 74.
  bool Reversed{};
  /// The height of each column, when the columns differ. Group 46.
  std::vector<double> Heights;

  auto operator==(const Columns &) const -> bool = default;
};

/// The drawing direction that takes the text style's own.
inline constexpr int Styled = 5;

/// The AcDbMText subclass of an MTEXT: its text with its formatting codes, its frame and its background.
struct DXFIO_API Paragraph
{
  /// The insertion point, in world coordinates. Groups 10, 20, 30.
  Vector Position{};
  /// The height of the letters, in drawing units. Group 40.
  double Height{};
  /// The width of the frame the text wraps in. Group 41.
  double Width{};
  /// The height of the frame, when the text has columns. Group 46, from R2007.
  double Defined{};
  /// The attachment point: 1 top left, 2 top center, 3 top right, 4 to 6 middle, 7 to 9 bottom. Group 71.
  int Attachment{1};
  /// The drawing direction: 1 left to right, 3 top to bottom, 5 as the text style has it. Group 72.
  int Flow{Styled};
  /// What the text says, with its formatting codes. Group 3 for each 250 characters but the last, then group 1.
  std::string Value;
  /// The name of the text style. Group 7.
  Symbol Style{"Standard"};
  /// The normal of the plane the entity is drawn in. Groups 210, 220, 230.
  Vector Extrusion{Up};
  /// The direction of the text's X axis, in world coordinates, when the file gives it rather than a rotation.
  /// Groups 11, 21, 31.
  std::optional<Vector> Direction;
  /// The rotation, when the file gives it rather than a direction. Group 50.
  std::optional<double> Rotation;
  /// The line spacing style: 1 at least, 2 exactly. Group 73.
  int Spacing{1};
  /// The line spacing, as a factor of the default. Group 44.
  double Factor{1};
  /// The background: 0 none, 1 a color, 2 the drawing window's color, and 16 a frame from R2018.
  /// Group 90, from R2004.
  int Fill{};
  /// The AutoCAD Color Index of the background. Group 63, from R2004.
  std::optional<int> Color;
  /// The true color of the background, as 0xRRGGBB. Group 421, from R2004.
  std::optional<int> Rgb;
  /// The color book of the background and the color's name in it, joined by a $, when the color comes from one.
  /// Group 431, from R2004.
  std::optional<std::string> Book;
  /// How far the background reaches past the text, as a factor of the letter height. Group 45, from R2004.
  std::optional<double> Margin;
  /// The transparency of the background. Group 441, from R2004.
  std::optional<int> Transparency;
  /// The columns, when the text has some. From R2018.
  std::optional<Entities::Columns> Columns;

  auto operator==(const Paragraph &) const -> bool = default;
};

/// An MTEXT: a paragraph of text that wraps in its frame, formatted by codes in the text.
struct DXFIO_API MText
{
  /// The layer, linetype, color and the rest every entity carries.
  Entities::Record Record{};
  /// The text, its frame and its background, from the AcDbMText subclass.
  Boxed<Entities::Paragraph> Paragraph;

  auto operator==(const MText &) const -> bool = default;
};
} // namespace DxfIO::Entities

#endif /* DXFIO_ENTITIES_MTEXT_HPP */
