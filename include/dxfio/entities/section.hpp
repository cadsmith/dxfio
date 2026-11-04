#ifndef DXFIO_ENTITIES_SECTION_HPP
#define DXFIO_ENTITIES_SECTION_HPP

#include "dxfio/api.hpp"
#include "dxfio/boxed.hpp"
#include "dxfio/entities/record.hpp"
#include "dxfio/handle.hpp"
#include "dxfio/vector.hpp"

#include <optional>
#include <string>
#include <vector>

namespace DxfIO::Entities
{
/// The AcDbSection subclass of a SECTIONOBJECT: the plane that cuts the model and the line it is drawn with.
struct DXFIO_API Cut
{
  /// What the section cuts with: 1 a plane, 2 a boundary, 4 a volume. Group 90.
  int State{1};
  /// Bits of the section's settings, which DXF does not spell out. Group 91.
  int Flags{};
  /// The name of the section. Group 1.
  std::string Name{};
  /// The vertical direction of the section plane. Groups 10, 20, 30.
  Vector Vertical{Up};
  /// The height of the top of the section's volume. Group 40.
  double Top{};
  /// The height of the bottom of the section's volume. Group 41.
  double Bottom{};
  /// The transparency of the plane's indicator. Group 70.
  int Transparency{};
  /// The AutoCAD Color Index of the plane's indicator. Group 62, from R2004.
  int Color{};
  /// The true color of the plane's indicator, as 0xRRGGBB. Group 420, from R2004.
  std::optional<int> Rgb{};
  /// The vertices of the section line; group 92 counts them. Groups 11, 21, 31.
  std::vector<Vector> Vertices{};
  /// The vertices of the back line; group 93 counts them. Groups 12, 22, 32.
  std::vector<Vector> Back{};
  /// The SECTIONSETTINGS object that sets how the section is drawn. Group 360.
  Handle Settings{};

  auto operator==(const Cut &) const -> bool = default;
};

/// A SECTIONOBJECT: a section plane that cuts through 3D solids and surfaces.
struct DXFIO_API SectionObject
{
  /// The layer, linetype, color and the rest every entity carries.
  Entities::Record Record{};
  /// The proxy graphics, for programs that cannot draw the entity. Group 92, or 160 from R2010, then 310.
  std::string Picture{};
  /// The plane, its line and its indicator, from the AcDbSection subclass.
  Boxed<Entities::Cut> Cut{};

  auto operator==(const SectionObject &) const -> bool = default;
};
} // namespace DxfIO::Entities

#endif /* DXFIO_ENTITIES_SECTION_HPP */
