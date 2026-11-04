#ifndef DXFIO_OBJECTS_SCALE_HPP
#define DXFIO_OBJECTS_SCALE_HPP

#include "dxfio/api.hpp"
#include "dxfio/base.hpp"

#include <string>

namespace DxfIO::Objects
{
/// A SCALE: one of the annotation scales the drawing lists, such as 1:50.
struct DXFIO_API Scale
{
  /// The handle, owner, reactors and extended data.
  DxfIO::Base Base{};
  /// Flags, 0 in what AutoCAD writes. Group 70.
  int Flags{};
  /// The name of the scale, such as "1:50". Group 300.
  std::string Name{};
  /// The paper units of the scale, 1 in 1:50. Group 140.
  double Paper{1};
  /// The drawing units of the scale, 50 in 1:50. Group 141.
  double Drawing{1};
  /// Whether this is the unit scale, 1:1. Group 290.
  bool Unit{};

  auto operator==(const Scale &) const -> bool = default;
};
} // namespace DxfIO::Objects

#endif /* DXFIO_OBJECTS_SCALE_HPP */
