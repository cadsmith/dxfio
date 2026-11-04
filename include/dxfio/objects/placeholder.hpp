#ifndef DXFIO_OBJECTS_PLACEHOLDER_HPP
#define DXFIO_OBJECTS_PLACEHOLDER_HPP

#include "dxfio/api.hpp"
#include "dxfio/base.hpp"

namespace DxfIO::Objects
{
/// An ACDBPLACEHOLDER: an object with no data of its own, standing for a name in a dictionary, such as a plot
/// style's.
struct DXFIO_API Placeholder
{
  /// The handle, owner, reactors and extended data.
  DxfIO::Base Base{};

  auto operator==(const Placeholder &) const -> bool = default;
};
} // namespace DxfIO::Objects

#endif /* DXFIO_OBJECTS_PLACEHOLDER_HPP */
