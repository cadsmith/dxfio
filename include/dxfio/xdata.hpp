#ifndef DXFIO_XDATA_HPP
#define DXFIO_XDATA_HPP

#include "dxfio/api.hpp"
#include "dxfio/group.hpp"

#include <string>
#include <vector>

namespace DxfIO
{
  /**
   *  The extended data one application attaches to a record: a 1001 group naming it, then its 1000
   *  to 1071 groups.
   */
  struct DXFIO_API Xdata
  {
    /**
     *  The application's name, as the APPID table registers it, such as "ACAD".
     */
    std::string Application{};
    /**
     *  The groups after the 1001, in their order.
     */
    std::vector<Group> Groups{};

    auto operator==(const Xdata &) const -> bool = default;
  };
} // namespace DxfIO

#endif /* DXFIO_XDATA_HPP */
