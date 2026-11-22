#ifndef DXFIO_APPEARANCE_HPP
#define DXFIO_APPEARANCE_HPP

#include "dxfio/api.hpp"
#include "dxfio/colors.hpp"
#include "dxfio/drawing.hpp"
#include "dxfio/entities/record.hpp"
#include "dxfio/lineweight.hpp"
#include "dxfio/symbol.hpp"

#include <optional>

namespace DxfIO
{
  /**
   *  How an entity is drawn, its ByLayer and ByBlock resolved.
   */
  struct DXFIO_API Appearance
  {
    /**
     *  The layer it is drawn on: an insert's for an entity on layer 0 inside a block.
     */
    Symbol Layer{"0"};
    /**
     *  An AutoCAD Color Index, 1 to 255.
     */
    int Color{Colors::White};
    /**
     *  The true color as 0xRRGGBB, when the color has one.
     */
    std::optional<int> Rgb{};
    /**
     *  The name of the linetype it is drawn with, never ByLayer or ByBlock.
     */
    Symbol Linetype{"Continuous"};
    /**
     *  The lineweight it is drawn with, never ByLayer or ByBlock.
     */
    DxfIO::Lineweight Lineweight{DxfIO::Lineweight::Default};

    auto operator==(const Appearance &) const -> bool = default;
  };

  /**
   *  ByLayer takes the layer's, and ByBlock outer's: the appearance of the insert an entity of a
   *  block is drawn through, or null for an entity drawn in a space, whose ByBlock is white,
   *  Continuous and the default lineweight. A layer the table does not hold is taken as white and
   *  Continuous with the default lineweight; a layer turned off keeps its color.
   */
  [[nodiscard]] DXFIO_API auto AppearanceOf(const Drawing &drawing, const Entities::Record &record,
                                            const Appearance *outer) -> Appearance;
} // namespace DxfIO

#endif /* DXFIO_APPEARANCE_HPP */
