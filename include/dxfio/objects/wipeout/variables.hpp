#ifndef DXFIO_OBJECTS_WIPEOUT_VARIABLES_HPP
#define DXFIO_OBJECTS_WIPEOUT_VARIABLES_HPP

#include "dxfio/api.hpp"
#include "dxfio/base.hpp"

namespace DxfIO::Objects
{
  /**
   *  A WIPEOUTVARIABLES: how the drawing shows its wipeouts.
   */
  struct DXFIO_API WipeoutVariables
  {
    /**
     *  The handle, owner, reactors and extended data.
     */
    DxfIO::Base Base{};
    /**
     *  1 to draw a frame around wipeouts, 0 not to. Group 70.
     */
    int Frame{};

    auto operator==(const WipeoutVariables &) const -> bool = default;
  };
} // namespace DxfIO::Objects

#endif /* DXFIO_OBJECTS_WIPEOUT_VARIABLES_HPP */
