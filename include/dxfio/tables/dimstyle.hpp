#ifndef DXFIO_TABLES_DIMSTYLE_HPP
#define DXFIO_TABLES_DIMSTYLE_HPP

#include "dxfio/api.hpp"
#include "dxfio/handle.hpp"
#include "dxfio/tables/record.hpp"

#include <string>

namespace DxfIO::Tables
{
/// The arrow size, text height and extension of a new dimension style.
inline constexpr double Size = 0.18;

/// The offset of extension lines from their origin in a new dimension style.
inline constexpr double Offset = 0.0625;

/// The spacing of baseline dimensions in a new dimension style.
inline constexpr double Increment = 0.38;

/// The center mark size and text gap of a new dimension style.
inline constexpr double Mark = 0.09;

/// Millimeters to the inch: the alternate unit factor of a new dimension style.
inline constexpr double Inch = 25.4;

/// The jog angle of radius dimensions, in degrees, in a new dimension style.
inline constexpr double Jog = 45;

/// The arrow blocks a dimension style names by their block names, as releases up to R14 write them.
struct DXFIO_API Arrows
{
  /// DIMBLK: the block drawn at both ends; empty for the closed filled arrow. Group 5.
  std::string Dimblk;
  /// DIMBLK1: the block at the first end when DIMSAH is on. Group 6.
  std::string Dimblk1;
  /// DIMBLK2: the block at the second end when DIMSAH is on. Group 7.
  std::string Dimblk2;

  auto operator==(const Arrows &) const -> bool = default;
};

/// A DIMSTYLE table record: the dimension variables a dimension is drawn with, each named after its own.
struct DXFIO_API Dimstyle
{
  /// The name, flags and handles every table record carries.
  Tables::Record Record{};
  /// The arrow blocks by name, written up to R14; later releases give them as handles.
  Tables::Arrows Arrows{};
  /// DIMPOST: the prefix and suffix of measurements, with <> where the measurement goes. Group 3.
  std::string Dimpost;
  /// DIMAPOST: the prefix and suffix of alternate measurements. Group 4.
  std::string Dimapost;
  /// DIMSCALE: the factor every size of the style is scaled by. Group 40.
  double Dimscale{1};
  /// DIMASZ: the size of arrowheads. Group 41.
  double Dimasz{Size};
  /// DIMEXO: how far extension lines start from the points they measure. Group 42.
  double Dimexo{Offset};
  /// DIMDLI: the spacing between the dimension lines of baseline dimensions. Group 43.
  double Dimdli{Increment};
  /// DIMEXE: how far extension lines run past the dimension line. Group 44.
  double Dimexe{Size};
  /// DIMRND: the value distances are rounded to; 0 for none. Group 45.
  double Dimrnd{};
  /// DIMDLE: how far the dimension line runs past extension lines when ticks are drawn. Group 46.
  double Dimdle{};
  /// DIMTP: the plus tolerance. Group 47.
  double Dimtp{};
  /// DIMTM: the minus tolerance. Group 48.
  double Dimtm{};
  /// DIMFXL: the length of extension lines when DIMFXLON fixes it. Group 49, from R2007.
  double Dimfxl{1};
  /// DIMJOGANG: the angle of the jog in jogged radius dimensions, in degrees. Group 50, from R2007.
  double Dimjogang{Jog};
  /// DIMTFILL: the text background: 0 none, 1 the drawing's background, 2 DIMTFILLCLR. Group 69, from R2007.
  int Dimtfill{};
  /// DIMTFILLCLR: the color of the text background. Group 70, from R2007.
  int Dimtfillclr{};
  /// DIMTOL: nonzero to append tolerances to measurements. Group 71.
  int Dimtol{};
  /// DIMLIM: nonzero to give measurements as limits. Group 72.
  int Dimlim{};
  /// DIMTIH: nonzero to keep text between extension lines horizontal. Group 73.
  int Dimtih{1};
  /// DIMTOH: nonzero to keep text outside extension lines horizontal. Group 74.
  int Dimtoh{1};
  /// DIMSE1: nonzero to leave out the first extension line. Group 75.
  int Dimse1{};
  /// DIMSE2: nonzero to leave out the second extension line. Group 76.
  int Dimse2{};
  /// DIMTAD: where text stands against the dimension line: 0 centered, 1 above, 2 on the far side, 3 JIS, 4 below.
  /// Group 77.
  int Dimtad{};
  /// DIMZIN: which zeros measurements drop: 0 to 3 for feet and inches, 4 leading, 8 trailing. Group 78.
  int Dimzin{};
  /// DIMAZIN: which zeros angles drop: 1 leading, 2 trailing, 3 both. Group 79, from R2000.
  int Dimazin{};
  /// DIMARCSYM: the arc length symbol: 0 before the text, 1 above it, 2 none. Group 90, from R2007.
  int Dimarcsym{};
  /// DIMTXT: the text height. Group 140.
  double Dimtxt{Size};
  /// DIMCEN: the size of center marks; negative to draw center lines, 0 for none. Group 141.
  double Dimcen{Mark};
  /// DIMTSZ: the size of ticks drawn in place of arrowheads; 0 for arrowheads. Group 142.
  double Dimtsz{};
  /// DIMALTF: the factor alternate units are measured by. Group 143.
  double Dimaltf{Inch};
  /// DIMLFAC: the factor linear measurements are scaled by. Group 144.
  double Dimlfac{1};
  /// DIMTVP: how far text stands above or below the dimension line when DIMTAD is 0. Group 145.
  double Dimtvp{};
  /// DIMTFAC: the height of tolerance text over the text height. Group 146.
  double Dimtfac{1};
  /// DIMGAP: the gap around text where it breaks the dimension line; negative to box the text. Group 147.
  double Dimgap{Mark};
  /// DIMALTRND: the value alternate measurements are rounded to. Group 148, from R2000.
  double Dimaltrnd{};
  /// DIMALT: nonzero to add alternate units to measurements. Group 170.
  int Dimalt{};
  /// DIMALTD: the decimal places of alternate units. Group 171.
  int Dimaltd{2};
  /// DIMTOFL: nonzero to draw the dimension line between extension lines when text is outside. Group 172.
  int Dimtofl{};
  /// DIMSAH: nonzero to take the arrows from DIMBLK1 and DIMBLK2 rather than DIMBLK. Group 173.
  int Dimsah{};
  /// DIMTIX: nonzero to keep text between extension lines. Group 174.
  int Dimtix{};
  /// DIMSOXD: nonzero to leave out dimension lines outside extension lines. Group 175.
  int Dimsoxd{};
  /// DIMCLRD: the color of dimension lines, arrowheads and leaders. Group 176.
  int Dimclrd{};
  /// DIMCLRE: the color of extension lines. Group 177.
  int Dimclre{};
  /// DIMCLRT: the color of text. Group 178.
  int Dimclrt{};
  /// DIMADEC: the decimal places of angles. Group 179, from R13.
  int Dimadec{};
  /// DIMDEC: the decimal places of measurements. Group 271, from R13.
  int Dimdec{4};
  /// DIMTDEC: the decimal places of tolerances. Group 272, from R13.
  int Dimtdec{4};
  /// DIMALTU: the format of alternate units, as DIMLUNIT. Group 273, from R13.
  int Dimaltu{2};
  /// DIMALTTD: the decimal places of alternate tolerances. Group 274, from R13.
  int Dimalttd{2};
  /// DIMAUNIT: the format of angles: 0 decimal degrees, 1 degrees minutes seconds, 2 gradians, 3 radians.
  /// Group 275, from R13.
  int Dimaunit{};
  /// DIMFRAC: how fractions stack: 0 horizontal, 1 diagonal, 2 not stacked. Group 276, from R13.
  int Dimfrac{};
  /// DIMLUNIT: the format of measurements: 1 scientific, 2 decimal, 3 engineering, 4 architectural, 5 fractional,
  /// 6 as Windows sets it. Group 277, from R13.
  int Dimlunit{2};
  /// DIMDSEP: the character that separates decimals. Group 278, from R13.
  int Dimdsep{'.'};
  /// DIMTMOVE: what moving the text does: 0 moves the dimension line, 1 adds a leader, 2 moves the text alone.
  /// Group 279, from R2000.
  int Dimtmove{};
  /// DIMJUST: where text stands along the dimension line: 0 centered, 1 and 2 by the first or second extension
  /// line, 3 and 4 above them. Group 280, from R13.
  int Dimjust{};
  /// DIMSD1: nonzero to leave out the first half of the dimension line. Group 281, from R13.
  int Dimsd1{};
  /// DIMSD2: nonzero to leave out the second half of the dimension line. Group 282, from R13.
  int Dimsd2{};
  /// DIMTOLJ: where tolerances stand against the measurement: 0 bottom, 1 middle, 2 top. Group 283, from R13.
  int Dimtolj{1};
  /// DIMTZIN: which zeros tolerances drop, as DIMZIN. Group 284, from R13.
  int Dimtzin{};
  /// DIMALTZ: which zeros alternate units drop, as DIMZIN. Group 285, from R13.
  int Dimaltz{};
  /// DIMALTTZ: which zeros alternate tolerances drop, as DIMZIN. Group 286, from R13.
  int Dimalttz{};
  /// DIMUPT: nonzero for the cursor to place the text as well as the dimension line. Group 288, from R13.
  int Dimupt{};
  /// DIMATFIT: what moves outside extension lines when text and arrows do not fit: 0 both, 1 arrows, 2 text,
  /// 3 whichever fits best. Group 289, from R2000.
  int Dimatfit{3};
  /// DIMFXLON: whether extension lines have the fixed length DIMFXL. Group 290, from R2007.
  bool Dimfxlon{};
  /// DIMTXSTY: the text style of the text. Group 340, from R13.
  Handle Dimtxsty{};
  /// DIMLDRBLK: the block record of leader arrows; none for the closed filled arrow. Group 341, from R13.
  Handle Dimldrblk{};
  /// DIMBLK: the block record of both arrows; none for the closed filled arrow. Group 342, from R13.
  Handle Dimblk{};
  /// DIMBLK1: the block record of the first arrow when DIMSAH is on. Group 343, from R13.
  Handle Dimblk1{};
  /// DIMBLK2: the block record of the second arrow when DIMSAH is on. Group 344, from R13.
  Handle Dimblk2{};
  /// DIMLTYPE: the linetype of dimension lines. Group 345, from R2007.
  Handle Dimltype{};
  /// DIMLTEX1: the linetype of the first extension line. Group 346, from R2007.
  Handle Dimltex1{};
  /// DIMLTEX2: the linetype of the second extension line. Group 347, from R2007.
  Handle Dimltex2{};
  /// DIMLWD: the lineweight of dimension lines, in hundredths of a millimeter, or -1 ByLayer, -2 ByBlock,
  /// -3 the default. Group 371, from R2000.
  int Dimlwd{-2};
  /// DIMLWE: the lineweight of extension lines, as DIMLWD. Group 372, from R2000.
  int Dimlwe{-2};

  auto operator==(const Dimstyle &) const -> bool = default;
};
} // namespace DxfIO::Tables

#endif /* DXFIO_TABLES_DIMSTYLE_HPP */
