#ifndef DXFIO_ENTITIES_DIMENSION_MEASURE_HPP
#define DXFIO_ENTITIES_DIMENSION_MEASURE_HPP

#include "dxfio/api.hpp"
#include "dxfio/symbol.hpp"
#include "dxfio/vector.hpp"

#include <optional>
#include <string>

namespace DxfIO::Entities
{
  /**
   *  The attachment point a new dimension's text takes: 5, middle center, as an MTEXT's group 71
   *  names it.
   */
  inline constexpr int MiddleCenter = 5;

  /**
   *  The measurement of a dimension not yet measured.
   */
  inline constexpr double Unmeasured = -1;

  /**
   *  The AcDbDimension subclass every kind of dimension shares: its block, its text and the plane
   *  it is drawn in.
   */
  struct DXFIO_API Measure
  {
    /**
     *  The version of the dimension's layout. Group 280, from R2010.
     */
    int Version{};
    /**
     *  The name of the anonymous block that draws the dimension. Group 2.
     */
    Symbol Block{};
    /**
     *  The definition point, in world coordinates; what it marks depends on the kind. Groups 10,
     *  20, 30.
     */
    Vector Point{};
    /**
     *  The middle of the text, in the entity's own coordinate system. Groups 11, 21, 31.
     */
    Vector Middle{};
    /**
     *  Where clones of a baseline or continued dimension are inserted, in the entity's own
     *  coordinate system. Groups 12, 22, 32.
     */
    std::optional<Vector> Clone{};
    /**
     *  The kind in the low three bits: 0 rotated, 1 aligned, 2 angular, 3 diameter, 4 radius, 5
     *  three-point angular, 6 ordinate; then 32 block of this dimension alone, 64 ordinate of X,
     *  128 text moved by the user. Group 70. A DIMENSION writes the low three bits from its form.
     */
    int Flags{};
    /**
     *  The text the user gave, where <> stands for the measurement; empty shows the measurement, a
     *  space nothing. Group 1.
     */
    std::string Text{};
    /**
     *  The attachment point of the text, as an MTEXT's: 1 top left to 9 bottom right. Group 71,
     *  from R2000.
     */
    int Attachment{MiddleCenter};
    /**
     *  The line spacing style of the text: 1 at least, 2 exactly. Group 72, from R2000.
     */
    std::optional<int> Spacing{};
    /**
     *  The line spacing of the text, as a factor of the default. Group 41, from R2000.
     */
    std::optional<double> Factor{};
    /**
     *  The measurement, as AutoCAD last computed it. Group 42, from R2000.
     */
    double Measured{Unmeasured};
    /**
     *  A flag DXF does not document. Group 73, from R2007.
     */
    int Above{};
    /**
     *  Whether the first arrowhead is flipped. Group 74, from R2007.
     */
    int First{};
    /**
     *  Whether the second arrowhead is flipped. Group 75, from R2007.
     */
    int Second{};
    /**
     *  The oblique angle of the extension lines of a linear dimension. Group 52.
     */
    std::optional<double> Oblique{};
    /**
     *  The rotation of the text away from its default direction. Group 53.
     */
    std::optional<double> Rotation{};
    /**
     *  The rotation of the block that draws the dimension. Group 54.
     */
    std::optional<double> Turned{};
    /**
     *  The horizontal direction of the text and lines: minus the angle from the OCS X axis to the
     *  UCS X axis. Group 51.
     */
    std::optional<double> Horizontal{};
    /**
     *  The normal of the plane the entity is drawn in. Groups 210, 220, 230.
     */
    Vector Normal{Up};
    /**
     *  The name of the dimension style. Group 3.
     */
    Symbol Style{};

    auto operator==(const Measure &) const -> bool = default;
  };
} // namespace DxfIO::Entities

#endif /* DXFIO_ENTITIES_DIMENSION_MEASURE_HPP */
