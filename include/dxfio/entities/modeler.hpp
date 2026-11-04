#ifndef DXFIO_ENTITIES_MODELER_HPP
#define DXFIO_ENTITIES_MODELER_HPP

#include "dxfio/api.hpp"
#include "dxfio/boxed.hpp"
#include "dxfio/entities/record.hpp"
#include "dxfio/group.hpp"
#include "dxfio/handle.hpp"

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace DxfIO::Entities
{
/// The AcDbModelerGeometry subclass: the ACIS body of a solid, a region or a surface.
struct DXFIO_API Geometry
{
  /// The version of the modeler's format. Group 70.
  std::optional<int> Version{};
  /// The lines of the ACIS body, as DXF encodes them, kept as groups; empty when the body is in ACDSDATA.
  /// Groups 1 and 3.
  std::vector<Group> Lines{};
  /// A flag of the R2013 layout, which DXF does not document. Group 290.
  std::optional<bool> Stored{};
  /// The GUID of the body, in the R2013 layout. Group 2.
  std::optional<std::string> Guid{};

  auto operator==(const Geometry &) const -> bool = default;
};

/// A REGION: a closed flat area, kept as an ACIS body.
struct DXFIO_API Region
{
  /// The layer, linetype, color and the rest every entity carries.
  Entities::Record Record{};
  /// The proxy graphics, for programs that cannot draw the entity. Group 92, or 160 from R2010, then 310.
  std::string Picture{};
  /// The ACIS body, from the AcDbModelerGeometry subclass.
  Boxed<Entities::Geometry> Geometry{};

  auto operator==(const Region &) const -> bool = default;
};

/// A BODY: an ACIS body that is neither a solid nor a region.
struct DXFIO_API Body
{
  /// The layer, linetype, color and the rest every entity carries.
  Entities::Record Record{};
  /// The proxy graphics, for programs that cannot draw the entity. Group 92, or 160 from R2010, then 310.
  std::string Picture{};
  /// The ACIS body, from the AcDbModelerGeometry subclass.
  Boxed<Entities::Geometry> Geometry{};

  auto operator==(const Body &) const -> bool = default;
};

/// A 3DSOLID: a solid, kept as an ACIS body.
struct DXFIO_API Solid3d
{
  /// The layer, linetype, color and the rest every entity carries.
  Entities::Record Record{};
  /// The proxy graphics, for programs that cannot draw the entity. Group 92, or 160 from R2010, then 310.
  std::string Picture{};
  /// The ACIS body, from the AcDbModelerGeometry subclass.
  Boxed<Entities::Geometry> Geometry{};
  /// The history of how the solid was made. Group 350, from R2007.
  Handle History{};

  auto operator==(const Solid3d &) const -> bool = default;
};

/// The kinds of surface entity.
enum class Sheet : std::uint8_t
{
  /// A SURFACE.
  Plain,
  /// A PLANESURFACE.
  Planar,
  /// A NURBSURFACE.
  Nurbs,
  /// An EXTRUDEDSURFACE.
  Extruded,
  /// A LOFTEDSURFACE.
  Lofted,
  /// A REVOLVEDSURFACE.
  Revolved,
  /// A SWEPTSURFACE.
  Swept,
};

/// A surface entity of the kind given: its ACIS body, the isolines drawn on it, and the kind's own groups.
template <Sheet KindV>
struct SurfaceOf
{
  /// The layer, linetype, color and the rest every entity carries.
  Entities::Record Record{};
  /// The proxy graphics, for programs that cannot draw the entity. Group 92, or 160 from R2010, then 310.
  std::string Picture{};
  /// The ACIS body, from the AcDbModelerGeometry subclass.
  Boxed<Entities::Geometry> Geometry{};
  /// The number of isolines drawn across the surface along U. Group 71.
  int Across{6};
  /// The number of isolines drawn across the surface along V. Group 72.
  int Along{6};
  /// The groups that follow the isolines, the kind's own data, kept as they came.
  std::vector<Group> Tail{};

  auto operator==(const SurfaceOf &) const -> bool = default;
};

/// A SURFACE: a surface of no more particular kind.
using Surface = SurfaceOf<Sheet::Plain>;
/// A PLANESURFACE: a flat surface.
using PlaneSurface = SurfaceOf<Sheet::Planar>;
/// A NURBSURFACE: a surface shaped by NURBS control vertices.
using NurbSurface = SurfaceOf<Sheet::Nurbs>;
/// An EXTRUDEDSURFACE: a profile extruded along a direction.
using ExtrudedSurface = SurfaceOf<Sheet::Extruded>;
/// A LOFTEDSURFACE: a surface through several cross sections.
using LoftedSurface = SurfaceOf<Sheet::Lofted>;
/// A REVOLVEDSURFACE: a profile turned about an axis.
using RevolvedSurface = SurfaceOf<Sheet::Revolved>;
/// A SWEPTSURFACE: a profile swept along a path.
using SweptSurface = SurfaceOf<Sheet::Swept>;
} // namespace DxfIO::Entities

#endif /* DXFIO_ENTITIES_MODELER_HPP */
