#ifndef DXFIO_SECTION_HPP
#define DXFIO_SECTION_HPP

#include "dxfio/api.hpp"
#include "dxfio/group.hpp"

#include <string>
#include <vector>

namespace DxfIO
{
  /**
   *  A section dxfio has no type for, such as THUMBNAILIMAGE or ACDSDATA, kept as its groups.
   */
  struct DXFIO_API Section
  {
    /**
     *  The name its 2 group gives, such as "THUMBNAILIMAGE".
     */
    std::string Name{};
    /**
     *  The groups between its name and its ENDSEC, in their order.
     */
    std::vector<Group> Groups{};

    auto operator==(const Section &) const -> bool = default;
  };
} // namespace DxfIO

#endif /* DXFIO_SECTION_HPP */
