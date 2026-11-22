#ifndef DXFIO_ENTITIES_LETTERING_HPP
#define DXFIO_ENTITIES_LETTERING_HPP

#include "dxfio/api.hpp"
#include "dxfio/symbol.hpp"
#include "dxfio/vector.hpp"

#include <cstdint>
#include <optional>
#include <string>

namespace DxfIO::Entities
{
  /**
   *  How a line of text lies along its alignment point, group 72.
   */
  // NOLINTNEXTLINE(performance-enum-size): a 16-bit group, any value round-trips
  enum class HorizontalAlignment : std::int16_t
  {
    Left,
    Center,
    Right,
    /**
     *  Between the position and the alignment point, its height scaled with its width.
     */
    Aligned,
    /**
     *  Centered both ways on the alignment point.
     */
    Middle,
    /**
     *  Between the position and the alignment point, its height kept.
     */
    Fit,
  };

  /**
   *  How a line of text lies across its alignment point, group 73 of a TEXT and 74 of an attribute.
   */
  // NOLINTNEXTLINE(performance-enum-size): a 16-bit group, any value round-trips
  enum class VerticalAlignment : std::int16_t
  {
    Baseline,
    Bottom,
    Middle,
    Top,
  };

  /**
   *  The AcDbText subclass a TEXT, an ATTRIB and an ATTDEF share: what the text says, where, and in
   *  which style.
   */
  struct DXFIO_API Lettering
  {
    /**
     *  How far the entity is extruded along its normal. Group 39.
     */
    double Thickness{};
    /**
     *  The first alignment point, where text aligned left starts, in the entity's own coordinate
     *  system. Groups 10, 20, 30.
     */
    Vector Position{};
    /**
     *  The height of the letters, in drawing units. Group 40.
     */
    double Height{};
    /**
     *  What the text says, with its control codes such as %%d. Group 1.
     */
    std::string Value{};
    /**
     *  The rotation, in degrees. Group 50.
     */
    double Rotation{};
    /**
     *  The relative width of the letters, 1 for the style's own. Group 41.
     */
    double Width{1};
    /**
     *  The slant of the letters, in degrees from the vertical. Group 51.
     */
    double Oblique{};
    /**
     *  The name of the text style. Group 7.
     */
    Symbol Style{"Standard"};
    /**
     *  How the text is mirrored: 2 backward, 4 upside down. Group 71.
     */
    int Generation{};
    /**
     *  Group 72.
     */
    HorizontalAlignment Horizontal{};
    /**
     *  The second alignment point, which justified text keeps to; none for text aligned left.
     *  Groups 11, 21, 31.
     */
    std::optional<Vector> Alignment{};
    /**
     *  The normal of the plane the entity is drawn in. Groups 210, 220, 230.
     */
    Vector Extrusion{Up};

    auto operator==(const Lettering &) const -> bool = default;
  };
} // namespace DxfIO::Entities

#endif /* DXFIO_ENTITIES_LETTERING_HPP */
