#ifndef DXFIO_ENTITY_HPP
#define DXFIO_ENTITY_HPP

#include "dxfio/api.hpp"
#include "dxfio/base.hpp"
#include "dxfio/entities/arc.hpp"
#include "dxfio/entities/attdef.hpp"
#include "dxfio/entities/circle.hpp"
#include "dxfio/entities/dimension/dimension.hpp"
#include "dxfio/entities/ellipse.hpp"
#include "dxfio/entities/face.hpp"
#include "dxfio/entities/hatch/hatch.hpp"
#include "dxfio/entities/helix.hpp"
#include "dxfio/entities/insert.hpp"
#include "dxfio/entities/leader.hpp"
#include "dxfio/entities/light.hpp"
#include "dxfio/entities/line.hpp"
#include "dxfio/entities/lwpolyline.hpp"
#include "dxfio/entities/mesh.hpp"
#include "dxfio/entities/mline.hpp"
#include "dxfio/entities/modeler.hpp"
#include "dxfio/entities/mtext.hpp"
#include "dxfio/entities/multileader.hpp"
#include "dxfio/entities/ole2frame.hpp"
#include "dxfio/entities/point.hpp"
#include "dxfio/entities/polyline.hpp"
#include "dxfio/entities/raster.hpp"
#include "dxfio/entities/ray.hpp"
#include "dxfio/entities/section.hpp"
#include "dxfio/entities/shape.hpp"
#include "dxfio/entities/solid.hpp"
#include "dxfio/entities/spline.hpp"
#include "dxfio/entities/table.hpp"
#include "dxfio/entities/text.hpp"
#include "dxfio/entities/tolerance.hpp"
#include "dxfio/entities/trace.hpp"
#include "dxfio/entities/underlay.hpp"
#include "dxfio/entities/viewport.hpp"
#include "dxfio/entities/xline.hpp"
#include "dxfio/unknown.hpp"

#include <string_view>
#include <variant>
#include <vector>

namespace DxfIO
{
/// An entity of the ENTITIES section or of a block: one of the kinds DXF defines, or an Unknown kept as it came.
using Entity =
    std::variant<Entities::Arc, Entities::Attdef, Entities::Body, Entities::Circle, Entities::ArcDimension,
                 Entities::DgnUnderlay, Entities::Dimension, Entities::DwfUnderlay, Entities::Ellipse,
                 Entities::ExtrudedSurface, Entities::Face3d, Entities::Hatch, Entities::Helix, Entities::Image,
                 Entities::Insert, Entities::LargeRadialDimension, Entities::Leader, Entities::Light, Entities::Line,
                 Entities::LoftedSurface, Entities::LwPolyline, Entities::Mesh, Entities::MultiLeader, Entities::MLine,
                 Entities::MText, Entities::NurbSurface, Entities::Ole2Frame, Entities::PdfUnderlay,
                 Entities::PlaneSurface, Entities::Point, Entities::Polyline, Entities::Ray, Entities::Region,
                 Entities::RevolvedSurface, Entities::Shape, Entities::SectionObject, Entities::Solid, Entities::Spline,
                 Entities::Surface, Entities::SweptSurface, Entities::AcadTable, Entities::Text, Entities::Tolerance,
                 Entities::Trace, Entities::Viewport, Entities::Solid3d, Entities::Wipeout, Entities::XLine, Unknown>;

/// The DXF name of an entity, such as "LINE", or the name an unknown entity came in with.
[[nodiscard]] DXFIO_API auto TypeOf(const Entity &one) -> std::string_view;

/// The handle, owner, reactors and extended data every entity carries.
/// The reference holds as long as the entity stays where it is: Edit::Add and Audit::Repair may move the entities of
/// the list they change.
[[nodiscard]] DXFIO_API auto BaseOf(const Entity &one) -> const Base &;

/// The handle, owner, reactors and extended data every entity carries, to change; it holds as the other BaseOf's does.
[[nodiscard]] DXFIO_API auto BaseOf(Entity &one) -> Base &;

namespace Entities
{
/// The entities of a section or a block, in the order they are drawn.
using List = std::vector<Entity>;
} // namespace Entities
} // namespace DxfIO

#endif /* DXFIO_ENTITY_HPP */
