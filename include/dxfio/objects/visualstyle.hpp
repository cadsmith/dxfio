#ifndef DXFIO_OBJECTS_VISUALSTYLE_HPP
#define DXFIO_OBJECTS_VISUALSTYLE_HPP

#include "dxfio/api.hpp"
#include "dxfio/base.hpp"
#include "dxfio/group.hpp"

#include <optional>
#include <string>
#include <vector>

namespace DxfIO::Objects
{
/// A VISUALSTYLE: how faces, edges and lighting are shown in a view, such as 2D wireframe or realistic.
/// Its properties change layout at R2010 and at R2013, so they are kept as groups.
struct DXFIO_API VisualStyle
{
  /// The handle, owner, reactors and extended data.
  DxfIO::Base Base{};
  /// The name of the style, such as "2dWireframe". Group 2.
  std::string Name{};
  /// The kind of style, such as 4 for 2D wireframe. Group 70.
  int Kind{};
  /// The version of the property layout, such as 2 in R2010 and 3 in R2018. Group 177, from R2010.
  std::optional<int> Version{};
  /// Whether the style is for AutoCAD's internal use only. Group 291, from R2010.
  std::optional<bool> Internal{};
  /// The properties that follow, as groups in the order they came.
  std::vector<DxfIO::Group> Properties{};

  auto operator==(const VisualStyle &) const -> bool = default;
};
} // namespace DxfIO::Objects

#endif /* DXFIO_OBJECTS_VISUALSTYLE_HPP */
