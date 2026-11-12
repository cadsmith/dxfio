#ifndef DXFIO_EDIT_SPACE_HPP
#define DXFIO_EDIT_SPACE_HPP

#include <cstdint>

namespace DxfIO::Edit
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
     *  The paper space of the first layout, the *Paper_Space block.
     */
    Paper,
  };
} // namespace DxfIO::Edit

#endif /* DXFIO_EDIT_SPACE_HPP */
