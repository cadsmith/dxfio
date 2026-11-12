#ifndef DXFIO_OBJECTS_MLEADER_STYLE_HPP
#define DXFIO_OBJECTS_MLEADER_STYLE_HPP

#include "dxfio/api.hpp"
#include "dxfio/base.hpp"
#include "dxfio/boxed.hpp"
#include "dxfio/handle.hpp"
#include "dxfio/lineweights.hpp"
#include "dxfio/vector.hpp"

#include <string>

namespace DxfIO::Objects::Mleader
{
  /**
   *  ByBlock as a packed color, AutoCAD's color in one integer: the color method 0xC1 in the top
   *  byte and nothing below it. Not Colors::Byblock, the index 0 of group 62.
   */
  inline constexpr int PackedByblock = -1056964608;

  /**
   *  The landing gap of a new multileader style.
   */
  inline constexpr double Margin = 2;

  /**
   *  The dogleg length of a new multileader style.
   */
  inline constexpr double Reach = 8;

  /**
   *  The arrow size and text height of a new multileader style.
   */
  inline constexpr double Size = 4;

  /**
   *  The break gap of a new multileader style.
   */
  inline constexpr double Breaking = 3.75;

  /**
   *  A text attachment above or below: centered on the text.
   */
  inline constexpr int Centered = 9;

  /**
   *  How the leader lines of a multileader style are drawn.
   */
  struct DXFIO_API Leader
  {
    /**
     *  Which end of a leader is drawn first: 0 the head, 1 the tail. Group 172.
     */
    int Order{};
    /**
     *  The most points a leader line has. Group 90.
     */
    int Points{2};
    /**
     *  The angle the first segment snaps to; 0 for any. Group 40.
     */
    double First{};
    /**
     *  The angle the second segment snaps to; 0 for any. Group 41.
     */
    double Second{};
    /**
     *  The kind of line: 0 invisible, 1 straight, 2 spline. Group 173.
     */
    int Type{1};
    /**
     *  The color of the lines, packed into one integer as AutoCAD does. Group 91.
     */
    int Color{PackedByblock};
    /**
     *  The linetype of the lines. Group 340.
     */
    Handle Linetype{};
    /**
     *  The lineweight in hundredths of a millimeter, or -1 ByLayer, -2 ByBlock, -3 the default.
     *  Group 92.
     */
    int Lineweight{Lineweights::Byblock};
    /**
     *  Whether the leader lands on the content. Group 290.
     */
    bool Landing{true};
    /**
     *  The gap between the landing and the content. Group 42.
     */
    double Gap{Margin};
    /**
     *  Whether the leader ends in a horizontal dogleg. Group 291.
     */
    bool Dogleg{true};
    /**
     *  The length of the dogleg. Group 43.
     */
    double Length{Reach};
    /**
     *  The block record of the arrowhead; none for the closed filled arrow. Group 341.
     */
    Handle Arrow{};
    /**
     *  The size of the arrowhead. Group 44.
     */
    double Size{Mleader::Size};

    auto operator==(const Leader &) const -> bool = default;
  };

  /**
   *  How the text of a multileader style is drawn.
   */
  struct DXFIO_API Text
  {
    /**
     *  The text a new multileader starts with. Group 300.
     */
    std::string Content{};
    /**
     *  The text style. Group 342.
     */
    Handle Style{};
    /**
     *  Where a leader from the left meets the text, such as 1 at the middle of its top line. Group
     *  174.
     */
    int Left{1};
    /**
     *  Where a leader from the right meets the text, as Left. Group 178.
     */
    int Right{1};
    /**
     *  How the text is turned: 0 along the leader, 1 horizontal, 2 along the leader but never
     *  upside down. Group 175.
     */
    int Angle{1};
    /**
     *  How the lines of the text are aligned: 0 left, 1 center, 2 right. Group 176.
     */
    int Alignment{};
    /**
     *  The color of the text, packed into one integer as AutoCAD does. Group 93.
     */
    int Color{PackedByblock};
    /**
     *  The height of the text. Group 45.
     */
    double Height{Size};
    /**
     *  Whether the text is framed. Group 292.
     */
    bool Frame{};
    /**
     *  Whether the text is always aligned left. Group 297.
     */
    bool Flush{};
    /**
     *  The space between the text and its alignment. Group 46.
     */
    double Space{Size};
    /**
     *  Which way leaders meet the text: 0 from the sides, 1 from above or below. Group 271, from
     *  R2010.
     */
    int Direction{};
    /**
     *  Where a leader from below meets the text. Group 272, from R2010.
     */
    int Bottom{Centered};
    /**
     *  Where a leader from above meets the text. Group 273, from R2010.
     */
    int Top{Centered};

    auto operator==(const Text &) const -> bool = default;
  };

  /**
   *  How the block of a multileader style is drawn, when its content is a block.
   */
  struct DXFIO_API Block
  {
    /**
     *  The block record of the content. Group 343.
     */
    Handle Record{};
    /**
     *  The color of the block, packed into one integer as AutoCAD does. Group 94.
     */
    int Color{PackedByblock};
    /**
     *  The scale of the block along X, Y and Z. Groups 47, 49, 140.
     */
    Vector Scale{.X = 1, .Y = 1, .Z = 1};
    /**
     *  Whether Scale applies. Group 293.
     */
    bool Scaled{true};
    /**
     *  The rotation of the block. Group 141.
     */
    double Rotation{};
    /**
     *  Whether Rotation applies. Group 294.
     */
    bool Rotated{true};
    /**
     *  Where the leader meets the block: 0 its extents, 1 its insertion point. Group 177.
     */
    int Connection{};

    auto operator==(const Block &) const -> bool = default;
  };
} // namespace DxfIO::Objects::Mleader

namespace DxfIO::Objects
{
  /**
   *  An MLEADERSTYLE: how a multileader draws its leader lines and its text or block, named by its
   *  entry in the multileader style dictionary.
   */
  struct DXFIO_API MLeaderStyle
  {
    /**
     *  The handle, owner, reactors and extended data.
     */
    DxfIO::Base Base{};
    /**
     *  A version number, 2 as AutoCAD writes it. Group 179, from R2010.
     */
    int Revision{2};
    /**
     *  The content at the end of the leader: 0 none, 1 a block, 2 text, 3 a tolerance. Group 170.
     */
    int Content{2};
    /**
     *  What is drawn first: 0 the content, 1 the leader. Group 171.
     */
    int Order{1};
    /**
     *  The style's description. Group 3.
     */
    std::string Description{};
    /**
     *  How the leader lines are drawn, kept on the heap as the text and block are, so every object
     *  stays small.
     */
    Boxed<Mleader::Leader> Leader{};
    /**
     *  How the text is drawn.
     */
    Boxed<Mleader::Text> Text{};
    /**
     *  How the block is drawn.
     */
    Boxed<Mleader::Block> Block{};
    /**
     *  The factor the whole multileader is scaled by. Group 142.
     */
    double Scale{1};
    /**
     *  Whether a property of the style was overridden. Group 295.
     */
    bool Overwrite{};
    /**
     *  Whether the style is annotative. Group 296.
     */
    bool Annotative{};
    /**
     *  The gap left where a leader is broken. Group 143.
     */
    double Breaks{Mleader::Breaking};
    /**
     *  A value AutoCAD writes with no published meaning, usually 0. Group 298, from R2013.
     */
    int Spare{};

    auto operator==(const MLeaderStyle &) const -> bool = default;
  };
} // namespace DxfIO::Objects

#endif /* DXFIO_OBJECTS_MLEADER_STYLE_HPP */
