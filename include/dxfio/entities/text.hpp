#ifndef DXFIO_ENTITIES_TEXT_HPP
#define DXFIO_ENTITIES_TEXT_HPP

#include "dxfio/api.hpp"
#include "dxfio/entities/lettering.hpp"
#include "dxfio/entities/record.hpp"

namespace DxfIO::Entities
{
  /**
   *  A TEXT: a single line of text in one style.
   */
  struct DXFIO_API Text
  {
    /**
     *  The layer, linetype, color and the rest every entity carries.
     */
    Entities::Record Record{};
    /**
     *  What the text says, where, and in which style.
     */
    Entities::Lettering Lettering{};
    /**
     *  The vertical justification: 0 baseline, 1 bottom, 2 middle, 3 top. Group 73.
     */
    int Vertical{};

    auto operator==(const Text &) const -> bool = default;
  };
} // namespace DxfIO::Entities

#endif /* DXFIO_ENTITIES_TEXT_HPP */
