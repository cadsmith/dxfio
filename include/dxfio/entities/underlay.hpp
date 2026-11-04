#ifndef DXFIO_ENTITIES_UNDERLAY_HPP
#define DXFIO_ENTITIES_UNDERLAY_HPP

#include "dxfio/api.hpp"
#include "dxfio/boxed.hpp"
#include "dxfio/entities/overlay.hpp"
#include "dxfio/entities/record.hpp"
#include "dxfio/handle.hpp"
#include "dxfio/vector.hpp"

#include <string>
#include <vector>

namespace DxfIO::Entities
{
/// The display flags of a new underlay: on, unclipped, in color.
inline constexpr int Visible = 2;

/// The full contrast a new underlay takes.
inline constexpr int Whole = 100;

/// A vertex of an underlay's clipping boundary.
struct DXFIO_API Bound
{
  /// The vertex, in the entity's own coordinate system. Groups 11, 21.
  Vector Position{};

  auto operator==(const Bound &) const -> bool = default;
};

/// The AcDbUnderlayReference subclass: where an underlay lies, how it is shown and how it is clipped.
struct DXFIO_API Overlaid
{
  /// The definition object that names the file. Group 340.
  Handle Definition{};
  /// The insertion point, in world coordinates. Groups 10, 20, 30.
  Vector Position{};
  /// The scale along each axis. Groups 41, 42, 43.
  Vector Scale{.X = 1, .Y = 1, .Z = 1};
  /// The rotation, in degrees. Group 50.
  double Rotation{};
  /// The normal of the plane the entity is drawn in. Groups 210, 220, 230.
  Vector Extrusion{Up};
  /// 1 clipped, 2 on, 4 monochrome, 8 adjusted for the background. Group 280.
  int Flags{Visible};
  /// The contrast, 20 to 100. Group 281.
  int Contrast{Whole};
  /// The fade, 0 to 80. Group 282.
  int Fade{};
  /// The vertices of the clipping boundary, two opposite corners for a rectangle.
  std::vector<Bound> Clip;

  auto operator==(const Overlaid &) const -> bool = default;
};

/// An underlay of the kind given: a PDF, DWF or DGN file shown under the drawing.
template <Overlay KindV>
struct UnderlayOf
{
  /// The layer, linetype, color and the rest every entity carries.
  Entities::Record Record{};
  /// The proxy graphics, for programs that cannot draw the entity. Group 92, or 160 from R2010, then 310.
  std::string Picture;
  /// Where the underlay lies and how it is shown, from the AcDbUnderlayReference subclass.
  Boxed<Entities::Overlaid> Overlaid;

  auto operator==(const UnderlayOf &) const -> bool = default;
};

/// A PDFUNDERLAY: a page of a PDF file shown under the drawing.
using PdfUnderlay = UnderlayOf<Overlay::Pdf>;
/// A DWFUNDERLAY: a sheet of a DWF file shown under the drawing.
using DwfUnderlay = UnderlayOf<Overlay::Dwf>;
/// A DGNUNDERLAY: a model of a MicroStation DGN file shown under the drawing.
using DgnUnderlay = UnderlayOf<Overlay::Dgn>;
} // namespace DxfIO::Entities

#endif /* DXFIO_ENTITIES_UNDERLAY_HPP */
