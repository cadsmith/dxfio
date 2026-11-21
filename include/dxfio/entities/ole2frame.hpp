#ifndef DXFIO_ENTITIES_OLE2FRAME_HPP
#define DXFIO_ENTITIES_OLE2FRAME_HPP

#include "dxfio/api.hpp"
#include "dxfio/boxed.hpp"
#include "dxfio/entities/record.hpp"
#include "dxfio/vector.hpp"

#include <cstddef>
#include <string>
#include <vector>

namespace DxfIO::Entities
{
  /**
   *  The AcDbOle2Frame subclass of an OLE2FRAME: the corners of the frame and the bytes of the OLE
   *  object.
   */
  struct DXFIO_API Ole2FrameData
  {
    /**
     *  The OLE version. Group 70.
     */
    int Version{2};
    /**
     *  The description of the object, such as the kind of document it is. Group 3.
     */
    std::string Description{};
    /**
     *  The upper-left corner, in world coordinates. Groups 10, 20, 30.
     */
    Vector Upper{};
    /**
     *  The lower-right corner, in world coordinates. Groups 11, 21, 31.
     */
    Vector Lower{};
    /**
     *  1 linked, 2 embedded, 3 static. Group 71.
     */
    int Kind{2};
    /**
     *  0 in model space, 1 in paper space. Group 72.
     */
    int Space{};
    /**
     *  The bytes of the OLE object. Groups 90 and 310.
     */
    std::vector<std::byte> Data{};

    auto operator==(const Ole2FrameData &) const -> bool = default;
  };

  /**
   *  An OLE2FRAME: an OLE object, such as a spreadsheet, placed in the drawing.
   */
  struct DXFIO_API Ole2Frame
  {
    /**
     *  The layer, linetype, color and the rest every entity carries.
     */
    Entities::Record Record{};
    /**
     *  The frame and the object, from the AcDbOle2Frame subclass.
     */
    Boxed<Entities::Ole2FrameData> Data{};

    auto operator==(const Ole2Frame &) const -> bool = default;
  };
} // namespace DxfIO::Entities

#endif /* DXFIO_ENTITIES_OLE2FRAME_HPP */
