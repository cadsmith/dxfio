#ifndef DXFIO_SPACE_HPP
#define DXFIO_SPACE_HPP

#include <cstdint>

namespace DxfIO
{
  /**
   *  Where an entity of the ENTITIES section is drawn.
   */
  enum class Space : std::uint8_t
  {
    /**
     *  Model space, the *Model_Space block.
     */
    Model,
    /**
     *  The paper space of the active layout, the *Paper_Space block.
     */
    Paper,
  };
} // namespace DxfIO

#endif /* DXFIO_SPACE_HPP */
