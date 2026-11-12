#ifndef DXFIO_ENTITIES_LETTERING_HPP
#define DXFIO_ENTITIES_LETTERING_HPP

#include "dxfio/api.hpp"
#include "dxfio/symbol.hpp"
#include "dxfio/vector.hpp"

#include <optional>
#include <string>

namespace DxfIO::Entities
{
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
     *  The horizontal justification: 0 left, 1 center, 2 right, 3 aligned, 4 middle, 5 fit. Group
     *  72.
     */
    int Horizontal{};
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
