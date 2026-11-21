#ifndef DXFIO_OBJECTS_MLEADER_STYLE_HPP
#define DXFIO_OBJECTS_MLEADER_STYLE_HPP

#include "dxfio/api.hpp"
#include "dxfio/base.hpp"
#include "dxfio/boxed.hpp"
#include "dxfio/colors.hpp"
#include "dxfio/handle.hpp"
#include "dxfio/lineweights.hpp"
#include "dxfio/vector.hpp"

#include <string>

namespace DxfIO::Objects::MLeaderStyles
{
  /**
   *  How the leader lines of a multileader style are drawn.
   */
  struct DXFIO_API LeaderFormat
  {
    /**
     *  Which end of a leader is drawn first: 0 the head, 1 the tail. Group 172.
     */
    int DrawOrder{};
    /**
     *  The most points a leader line has. Group 90.
     */
    int MaxPoints{2};
    /**
     *  The angle the first segment snaps to; 0 for any. Group 40.
     */
    double FirstSegmentAngle{};
    /**
     *  The angle the second segment snaps to; 0 for any. Group 41.
     */
    double SecondSegmentAngle{};
    /**
     *  The kind of line: 0 invisible, 1 straight, 2 spline. Group 173.
     */
    int LeaderType{1};
    /**
     *  The color of the lines, packed into one integer as AutoCAD does. Group 91.
     */
    int Color{Colors::PackedByBlock};
    /**
     *  The linetype of the lines. Group 340.
     */
    Handle Linetype{};
    /**
     *  The lineweight in hundredths of a millimeter, or -1 ByLayer, -2 ByBlock, -3 the default.
     *  Group 92.
     */
    int Lineweight{Lineweights::ByBlock};
    /**
     *  Whether the leader lands on the content. Group 290.
     */
    bool EnableLanding{true};
    /**
     *  The gap between the landing and the content. Group 42.
     */
    double LandingGap{2};
    /**
     *  Whether the leader ends in a horizontal dogleg. Group 291.
     */
    bool EnableDogleg{true};
    /**
     *  The length of the dogleg. Group 43.
     */
    double DoglegLength{8};
    /**
     *  The block record of the arrowhead; none for the closed filled arrow. Group 341.
     */
    Handle Arrowhead{};
    /**
     *  The size of the arrowhead. Group 44.
     */
    double ArrowheadSize{4};

    auto operator==(const LeaderFormat &) const -> bool = default;
  };

  /**
   *  How the text of a multileader style is drawn.
   */
  struct DXFIO_API TextFormat
  {
    /**
     *  The text a new multileader starts with. Group 300.
     */
    std::string DefaultText{};
    /**
     *  The text style. Group 342.
     */
    Handle Style{};
    /**
     *  Where a leader from the left meets the text, such as 1 at the middle of its top line. Group
     *  174.
     */
    int LeftAttachment{1};
    /**
     *  Where a leader from the right meets the text, as LeftAttachment. Group 178.
     */
    int RightAttachment{1};
    /**
     *  How the text is turned: 0 along the leader, 1 horizontal, 2 along the leader but never
     *  upside down. Group 175.
     */
    int AngleType{1};
    /**
     *  How the lines of the text are aligned: 0 left, 1 center, 2 right. Group 176.
     */
    int Alignment{};
    /**
     *  The color of the text, packed into one integer as AutoCAD does. Group 93.
     */
    int Color{Colors::PackedByBlock};
    /**
     *  The height of the text. Group 45.
     */
    double Height{4};
    /**
     *  Whether the text is framed. Group 292.
     */
    bool Frame{};
    /**
     *  Whether the text is always aligned left. Group 297.
     */
    bool AlwaysLeft{};
    /**
     *  The space between the text and its alignment. Group 46.
     */
    double AlignSpace{4};
    /**
     *  Which way leaders meet the text: 0 from the sides, 1 from above or below. Group 271, from
     *  R2010.
     */
    int AttachmentDirection{};
    /**
     *  Where a leader from below meets the text. Group 272, from R2010.
     */
    int BottomAttachment{9};
    /**
     *  Where a leader from above meets the text. Group 273, from R2010.
     */
    int TopAttachment{9};

    auto operator==(const TextFormat &) const -> bool = default;
  };

  /**
   *  How the block of a multileader style is drawn, when its content is a block.
   */
  struct DXFIO_API BlockFormat
  {
    /**
     *  The block record of the content. Group 343.
     */
    Handle Content{};
    /**
     *  The color of the block, packed into one integer as AutoCAD does. Group 94.
     */
    int Color{Colors::PackedByBlock};
    /**
     *  The scale of the block along X, Y and Z. Groups 47, 49, 140.
     */
    Vector Scale{.X = 1, .Y = 1, .Z = 1};
    /**
     *  Whether Scale applies. Group 293.
     */
    bool EnableScale{true};
    /**
     *  The rotation of the block. Group 141.
     */
    double Rotation{};
    /**
     *  Whether Rotation applies. Group 294.
     */
    bool EnableRotation{true};
    /**
     *  Where the leader meets the block: 0 its extents, 1 its insertion point. Group 177.
     */
    int Connection{};

    auto operator==(const BlockFormat &) const -> bool = default;
  };
} // namespace DxfIO::Objects::MLeaderStyles

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
    int ContentType{2};
    /**
     *  What is drawn first: 0 the content, 1 the leader. Group 171.
     */
    int DrawOrder{1};
    /**
     *  The style's description. Group 3.
     */
    std::string Description{};
    /**
     *  How the leader lines are drawn, kept on the heap as the text and block are, so every object
     *  stays small.
     */
    Boxed<MLeaderStyles::LeaderFormat> Leader{};
    /**
     *  How the text is drawn.
     */
    Boxed<MLeaderStyles::TextFormat> Text{};
    /**
     *  How the block is drawn.
     */
    Boxed<MLeaderStyles::BlockFormat> Block{};
    /**
     *  The factor the whole multileader is scaled by. Group 142.
     */
    double Scale{1};
    /**
     *  Whether a property of the style was overridden. Group 295.
     */
    bool OverwriteProperty{};
    /**
     *  Whether the style is annotative. Group 296.
     */
    bool Annotative{};
    /**
     *  The gap left where a leader is broken. Group 143.
     */
    double BreakGapSize{3.75};
    /**
     *  Unnamed in the DXF reference; 0 as AutoCAD writes it. Group 298, from R2013.
     */
    int Unknown1{};

    auto operator==(const MLeaderStyle &) const -> bool = default;
  };
} // namespace DxfIO::Objects

#endif /* DXFIO_OBJECTS_MLEADER_STYLE_HPP */
