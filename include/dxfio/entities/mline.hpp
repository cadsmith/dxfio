#ifndef DXFIO_ENTITIES_MLINE_HPP
#define DXFIO_ENTITIES_MLINE_HPP

#include "dxfio/api.hpp"
#include "dxfio/boxed.hpp"
#include "dxfio/entities/record.hpp"
#include "dxfio/handle.hpp"
#include "dxfio/symbol.hpp"
#include "dxfio/vector.hpp"

#include <string>
#include <vector>

namespace DxfIO::Entities
{
/// How one line of a multiline runs along a segment: where its pieces lie and where its fill lies.
struct DXFIO_API Strand
{
  /// The parameters of the line's pieces along the segment; group 74 counts them. Group 41.
  std::vector<double> Spans;
  /// The parameters of the line's area fill; group 75 counts them. Group 42.
  std::vector<double> Fills;

  auto operator==(const Strand &) const -> bool = default;
};

/// A vertex of a multiline, with the segment that starts at it.
struct DXFIO_API Joint
{
  /// The vertex, in world coordinates. Groups 11, 21, 31.
  Vector Position{};
  /// The direction of the segment that starts at the vertex. Groups 12, 22, 32.
  Vector Direction{};
  /// The direction of the miter at the vertex. Groups 13, 23, 33.
  Vector Miter{};
  /// How each line of the style runs along the segment, in the style's order.
  std::vector<Strand> Strands;

  auto operator==(const Joint &) const -> bool = default;
};

/// The AcDbMline subclass of an MLINE: its style, its justification and its vertices.
struct DXFIO_API Web
{
  /// The name of the multiline style. Group 2.
  Symbol Style;
  /// The MLINESTYLE object. Group 340.
  Handle Styled{};
  /// The scale of the style's offsets. Group 40.
  double Scale{1};
  /// 0 top, 1 zero, 2 bottom. Group 70.
  int Justification{};
  /// 1 has vertices, 2 closed, 4 start caps suppressed, 8 end caps suppressed. Group 71.
  int Flags{};
  /// The number of lines in the style. Group 73.
  int Lines{};
  /// The start point, in world coordinates. Groups 10, 20, 30.
  Vector Start{};
  /// The normal of the plane the entity is drawn in. Groups 210, 220, 230.
  Vector Normal{Up};
  /// The vertices; group 72 counts them.
  std::vector<Joint> Joints;

  auto operator==(const Web &) const -> bool = default;
};

/// An MLINE: several parallel lines drawn together, as a multiline style sets them.
struct DXFIO_API MLine
{
  /// The layer, linetype, color and the rest every entity carries.
  Entities::Record Record{};
  /// The style, justification and vertices, from the AcDbMline subclass.
  Boxed<Entities::Web> Web;

  auto operator==(const MLine &) const -> bool = default;
};
} // namespace DxfIO::Entities

#endif /* DXFIO_ENTITIES_MLINE_HPP */
