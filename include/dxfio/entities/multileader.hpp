#ifndef DXFIO_ENTITIES_MULTILEADER_HPP
#define DXFIO_ENTITIES_MULTILEADER_HPP

#include "dxfio/api.hpp"
#include "dxfio/boxed.hpp"
#include "dxfio/entities/record.hpp"
#include "dxfio/handle.hpp"
#include "dxfio/matrix.hpp"
#include "dxfio/vector.hpp"

#include <optional>
#include <string>
#include <vector>

namespace DxfIO::Entities
{
/// ByLayer, as a packed color: the color method 0xC0 in the top byte and nothing below it.
inline constexpr int Inherited = -1073741824;

/// A break in a leader line, between two points of one of its segments.
struct DXFIO_API Gap
{
  /// The segment the break is in. Group 90.
  int Index{};
  /// Where the break starts, in world coordinates. Groups 11, 21, 31.
  Vector Start{};
  /// Where the break ends, in world coordinates. Groups 12, 22, 32.
  Vector End{};

  auto operator==(const Gap &) const -> bool = default;
};

/// A leader line of a MULTILEADER: its vertices, its breaks and what it overrides of its MULTILEADER.
struct DXFIO_API Stem
{
  /// The vertices, in world coordinates. Groups 10, 20, 30.
  std::vector<Vector> Points{};
  /// The breaks in its segments, each opened by group 90.
  std::vector<Gap> Gaps{};
  /// The line's index in its leader. Group 91.
  int Index{};
  /// The color of the line, as a packed color, when it has its own. Group 92.
  std::optional<int> Color{};
  /// The line type, when it has its own: 0 invisible, 1 straight, 2 spline. Group 170.
  std::optional<int> Kind{};
  /// The linetype of the line, when it has its own. Group 340.
  Handle Linetype{};
  /// The lineweight of the line, when it has its own. Group 171.
  std::optional<int> Lineweight{};
  /// The size of the arrowhead, when it has its own. Group 40.
  std::optional<double> Arrow{};
  /// The block that draws the arrowhead, when it has its own. Group 341.
  Handle Head{};
  /// Which properties the line has of its own, as bits. Group 94.
  std::optional<int> Overrides{};

  auto operator==(const Stem &) const -> bool = default;
};

/// A break in the dogleg of a leader.
struct DXFIO_API Span
{
  /// Where the break starts, in world coordinates. Groups 12, 22, 32.
  Vector Start{};
  /// Where the break ends, in world coordinates. Groups 13, 23, 33.
  Vector End{};

  auto operator==(const Span &) const -> bool = default;
};

/// A leader of a MULTILEADER: its landing, its dogleg and the leader lines that run from it.
struct DXFIO_API Branch
{
  /// Whether the point the lines end at is set. Group 290.
  bool Lasting{true};
  /// Whether the direction of the dogleg is set. Group 291.
  bool Bent{true};
  /// The point the leader lines end at, where the landing starts, in world coordinates. Groups 10, 20, 30.
  Vector Last{};
  /// The direction of the dogleg. Groups 11, 21, 31.
  Vector Dogleg{.X = 1};
  /// The breaks in the dogleg, each opened by group 12.
  std::vector<Span> Spans{};
  /// The leader's index. Group 90.
  int Index{};
  /// The length of the dogleg. Group 40.
  double Length{};
  /// The leader lines, each between group 304 LEADER_LINE{ and group 305.
  std::vector<Stem> Stems{};
  /// How the leader attaches to the content: 0 horizontally, 1 vertically. Group 271, from R2010.
  int Attachment{};

  auto operator==(const Branch &) const -> bool = default;
};

/// The context data of a MULTILEADER: where its content and leaders lie, and how its text or block is drawn.
struct DXFIO_API Note
{
  /// The overall scale of the content. Group 40.
  double Scale{1};
  /// The base point of the content, in world coordinates. Groups 10, 20, 30.
  Vector Base{};
  /// The height of the text. Group 41.
  double Height{};
  /// The size of the arrowheads. Group 140.
  double Arrow{};
  /// The gap between the landing and the text. Group 145.
  double Landing{};
  /// How text attaches to a leader on its left. Group 174.
  int Left{1};
  /// How text attaches to a leader on its right. Group 175.
  int Right{1};
  /// How the text is aligned. Group 176.
  int Align{};
  /// How the content attaches, as AutoCAD records it. Group 177.
  int Attach{};
  /// Whether the content is text, which the groups from Text to Breaks then describe. Group 290.
  bool Texted{};
  /// What the text says, with its MTEXT formatting codes. Group 304.
  std::string Text{};
  /// The normal of the text's plane. Groups 11, 21, 31.
  Vector Normal{Up};
  /// The text style. Group 340.
  Handle Style{};
  /// Where the text lies, in world coordinates. Groups 12, 22, 32.
  Vector Location{};
  /// The direction of the text. Groups 13, 23, 33.
  Vector Direction{.X = 1};
  /// The rotation of the text. Group 42.
  double Rotation{};
  /// The width of the text's frame. Group 43.
  double Width{};
  /// The height of the text's frame. Group 44.
  double Tall{};
  /// The line spacing, as a factor of the default. Group 45.
  double Spacing{1};
  /// The line spacing style: 1 at least, 2 exactly. Group 170.
  int Spaced{1};
  /// The color of the text, as a packed color. Group 90.
  int Color{Inherited};
  /// The attachment point of the text, as an MTEXT's group 71 gives it. Group 171.
  int Attachment{1};
  /// The drawing direction of the text, as an MTEXT's group 72 gives it. Group 172.
  int Flow{5};
  /// The color of the background, as a packed color. Group 91.
  int Background{Inherited};
  /// How far the background reaches past the text, as a factor of the letter height. Group 141.
  double Margin{};
  /// The transparency of the background. Group 92.
  int Transparency{};
  /// Whether the text has a background fill. Group 291.
  bool Backed{};
  /// Whether the background is filled with the drawing window's color. Group 292.
  bool Filled{};
  /// The columns: 0 none, 1 static, 2 dynamic. Group 173.
  int Columns{};
  /// Whether the height of the text's frame follows the text. Group 293.
  bool Grows{};
  /// The width of a column. Group 142.
  double Column{};
  /// The gap between columns. Group 143.
  double Gutter{};
  /// Whether the text flows through the columns in reverse. Group 294.
  bool Reversed{};
  /// The height of each column. Group 144.
  std::vector<double> Heights{};
  /// Whether words break at the edge of the frame. Group 295.
  bool Breaks{};
  /// Whether the content is a block, which the groups from Block to Frame then describe. Group 296.
  bool Blocked{};
  /// The block record of the content. Group 341.
  Handle Block{};
  /// The normal of the block's plane. Groups 14, 24, 34.
  Vector Facing{Up};
  /// Where the block lies, in world coordinates. Groups 15, 25, 35.
  Vector Placed{};
  /// The scale of the block along each axis. Groups 16, 26, 36.
  Vector Sized{.X = 1, .Y = 1, .Z = 1};
  /// The rotation of the block. Group 46.
  double Turned{};
  /// The color of the block, as a packed color. Group 93.
  int Tint{Inherited};
  /// The block's transformation matrix, its 16 cells each a group 47.
  Matrix Frame{};
  /// The origin of the MULTILEADER's plane, in world coordinates. Groups 110, 120, 130.
  Vector Origin{};
  /// The X axis of the MULTILEADER's plane. Groups 111, 121, 131.
  Vector Across{.X = 1};
  /// The Y axis of the MULTILEADER's plane. Groups 112, 122, 132.
  Vector Along{.Y = 1};
  /// Whether the normal of the MULTILEADER's plane is reversed. Group 297.
  bool Flipped{};
  /// The leaders, each between group 302 LEADER{ and group 303.
  std::vector<Branch> Branches{};
  /// How text attaches to a leader below it, 9 at its center. Group 272, from R2010.
  int Bottom{9};
  /// How text attaches to a leader above it, 9 at its center. Group 273, from R2010.
  int Top{9};

  auto operator==(const Note &) const -> bool = default;
};

/// An arrowhead of a MULTILEADER, by the index of the leader line it ends.
struct DXFIO_API Tip
{
  /// Which arrowhead of the MULTILEADER this is, by index. Group 94.
  int Index{};
  /// The block that draws the arrowhead. Group 345.
  Handle Block{};

  auto operator==(const Tip &) const -> bool = default;
};

/// The value a MULTILEADER gives an attribute of its block.
struct DXFIO_API Label
{
  /// The ATTDEF of the block the value is for. Group 330.
  Handle Definition{};
  /// Which attribute of the block the value is for, by index. Group 177.
  int Index{};
  /// The width of the attribute's text. Group 44.
  double Width{};
  /// The value the attribute takes. Group 302.
  std::string Text{};

  auto operator==(const Label &) const -> bool = default;
};

/// The AcDbMLeader subclass of a MULTILEADER: its context data, and what it overrides of its MLEADERSTYLE.
struct DXFIO_API Callout
{
  /// The version of the MULTILEADER's layout. Group 270, from R2010.
  int Version{2};
  /// Where the content and the leaders lie, between group 300 CONTEXT_DATA{ and group 301.
  Note Context{};
  /// The MLEADERSTYLE. Group 340.
  Handle Style{};
  /// Which properties of the style the MULTILEADER overrides, as bits. Group 90.
  int Overrides{};
  /// The leader line type: 0 invisible, 1 straight, 2 spline. Group 170.
  int Line{1};
  /// The color of the leader lines, as a packed color. Group 91.
  int Color{Inherited};
  /// The linetype of the leader lines. Group 341.
  Handle Linetype{};
  /// The lineweight of the leader lines, as an entity's group 370 gives it. Group 171.
  int Lineweight{-2};
  /// Whether the leaders have a landing. Group 290.
  bool Landing{true};
  /// Whether the leaders have a dogleg. Group 291.
  bool Bent{true};
  /// The length of the dogleg. Group 41.
  double Length{};
  /// The block that draws the arrowheads; none for the default arrowhead. Group 342.
  Handle Arrow{};
  /// The size of the arrowheads. Group 42.
  double Size{};
  /// The content: 0 none, 1 a block, 2 an MTEXT, 3 a tolerance. Group 172.
  int Content{2};
  /// The text style. Group 343.
  Handle Lettering{};
  /// How text attaches to a leader on its left. Group 173.
  int Left{1};
  /// How text attaches to a leader on its right. Group 95.
  int Right{1};
  /// How the text is turned: 0 along the last leader segment, 1 horizontal, 2 along it but kept upright. Group 174.
  int Angle{1};
  /// The alignment of the text: 0 left, 1 center, 2 right. Group 175.
  int Align{};
  /// The color of the text, as a packed color. Group 92.
  int Tint{Inherited};
  /// Whether the text has a frame. Group 292.
  bool Framed{};
  /// The block record of the content. Group 344.
  Handle Block{};
  /// The color of the block, as a packed color. Group 93.
  int Shade{Inherited};
  /// The scale of the block along each axis. Groups 10, 20, 30.
  Vector Sized{.X = 1, .Y = 1, .Z = 1};
  /// The rotation of the block. Group 43.
  double Turned{};
  /// Where the block meets the leaders: 0 the center of its extents, 1 its insertion point. Group 176.
  int Connection{};
  /// Whether the MULTILEADER is annotative. Group 293.
  bool Annotative{};
  /// The arrowheads of the leader lines, each opened by group 94.
  std::vector<Tip> Tips{};
  /// The values of the block's attributes, each opened by group 330.
  std::vector<Label> Labels{};
  /// Whether the text runs in the negative direction. Group 294.
  bool Negative{};
  /// The alignment of the text while it is edited in place. Group 178.
  int Pinned{};
  /// The attachment point of the text: 1 left, 2 center, 3 right. Group 179.
  int Point{1};
  /// The overall scale. Group 45.
  double Factor{1};
  /// How text attaches to the leaders: 0 horizontally, 1 vertically. Group 271, from R2010.
  int Attachment{};
  /// How text attaches to a leader below it, 9 at its center. Group 272, from R2010.
  int Bottom{9};
  /// How text attaches to a leader above it, 9 at its center. Group 273, from R2010.
  int Top{9};
  /// Whether the leaders extend to the text. Group 295, from R2013.
  bool Extended{};

  auto operator==(const Callout &) const -> bool = default;
};

/// A MULTILEADER: one or more leaders that point from a text, a block or a tolerance.
struct DXFIO_API MultiLeader
{
  /// The layer, linetype, color and the rest every entity carries.
  Entities::Record Record{};
  /// The proxy graphics, for programs that cannot draw the entity. Group 92, or 160 from R2010, then 310.
  std::string Picture{};
  /// The leaders, their content and what they override of their style, from the AcDbMLeader subclass.
  Boxed<Entities::Callout> Callout{};

  auto operator==(const MultiLeader &) const -> bool = default;
};
} // namespace DxfIO::Entities

#endif /* DXFIO_ENTITIES_MULTILEADER_HPP */
