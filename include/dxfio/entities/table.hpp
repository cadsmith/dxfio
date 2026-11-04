#ifndef DXFIO_ENTITIES_TABLE_HPP
#define DXFIO_ENTITIES_TABLE_HPP

#include "dxfio/api.hpp"
#include "dxfio/entities/record.hpp"
#include "dxfio/group.hpp"
#include "dxfio/symbol.hpp"
#include "dxfio/vector.hpp"

#include <string>
#include <vector>

namespace DxfIO::Entities
{
/// An ACAD_TABLE: a table of rows and cells, drawn through an anonymous block it references.
struct DXFIO_API AcadTable
{
  /// The layer, linetype, color and the rest every entity carries.
  Entities::Record Record{};
  /// The proxy graphics, for programs that cannot draw the entity. Group 92, or 160 from R2010, then 310.
  std::string Picture{};
  /// The name of the anonymous block that draws the table. Group 2.
  Symbol Block{""};
  /// The insertion point. Groups 10, 20, 30.
  Vector Position{};
  /// The scale along each axis. Groups 41, 42, 43.
  Vector Scale{.X = 1, .Y = 1, .Z = 1};
  /// The rotation, in degrees. Group 50.
  double Rotation{};
  /// The normal of the plane the entity is drawn in. Groups 210, 220, 230.
  Vector Extrusion{Up};
  /// The groups that follow, the table's style, rows, columns and cells, kept as they came.
  std::vector<Group> Body{};

  auto operator==(const AcadTable &) const -> bool = default;
};
} // namespace DxfIO::Entities

#endif /* DXFIO_ENTITIES_TABLE_HPP */
