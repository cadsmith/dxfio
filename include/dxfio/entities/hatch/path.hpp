#ifndef DXFIO_ENTITIES_HATCH_PATH_HPP
#define DXFIO_ENTITIES_HATCH_PATH_HPP

#include "dxfio/api.hpp"
#include "dxfio/entities/hatch/edge.hpp"
#include "dxfio/handle.hpp"

#include <vector>

namespace DxfIO::Entities
{
/// A boundary path of a HATCH: a polyline or a loop of edges, and the entities it was made from.
struct DXFIO_API Path
{
  /// 1 external, 2 polyline, 4 derived, 8 textbox, 16 outermost. Group 92. Flag 2 is written from what the path
  /// holds: set when it has vertices, cleared when it has edges.
  int Flags{};
  /// 1 when the polyline has bulges. Group 72.
  int Bulged{};
  /// 1 when the polyline is closed. Group 73.
  int Closed{};
  /// The vertices of a polyline path; group 93 counts them.
  std::vector<Edges::Node> Vertices{};
  /// The edges of any other path; group 93 counts them.
  std::vector<Edges::Edge> Edges{};
  /// The entities the boundary was made from, when the hatch is associative; group 97 counts them. Group 330.
  std::vector<Handle> Sources{};

  auto operator==(const Path &) const -> bool = default;
};
} // namespace DxfIO::Entities

#endif /* DXFIO_ENTITIES_HATCH_PATH_HPP */
