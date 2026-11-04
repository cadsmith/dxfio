#ifndef DXFIO_ENTITIES_RASTER_HPP
#define DXFIO_ENTITIES_RASTER_HPP

#include "dxfio/api.hpp"
#include "dxfio/boxed.hpp"
#include "dxfio/entities/record.hpp"
#include "dxfio/handle.hpp"
#include "dxfio/vector.hpp"

#include <string>
#include <vector>

namespace DxfIO::Entities
{
/// The display flags of a new image or wipeout, as AutoCAD writes them: shown, shown when not aligned with the
/// screen, and clipped.
inline constexpr int Displayed = 7;

/// The brightness and the contrast of a new image or wipeout: 50, halfway, as AutoCAD writes them.
inline constexpr int Halfway = 50;

/// A vertex of the clipping boundary of an image or a wipeout.
struct DXFIO_API Pixel
{
  /// The vertex, in pixels of the image. Groups 14, 24.
  Vector Position{};

  auto operator==(const Pixel &) const -> bool = default;
};

/// What an IMAGE and a WIPEOUT share: where the picture lies, how it is shown and how it is clipped.
struct DXFIO_API Raster
{
  /// The version of the image's layout. Group 90.
  int Version{};
  /// The insertion point, the outer corner of the bottom-left pixel, in world coordinates. Groups 10, 20, 30.
  Vector Origin{};
  /// One pixel's step along the bottom edge of the image, in world coordinates. Groups 11, 21, 31.
  Vector Across{};
  /// One pixel's step up the left edge of the image, from the insertion point, in world coordinates.
  /// Groups 12, 22, 32.
  Vector Down{};
  /// The width and height of the image, in pixels. Groups 13, 23.
  Vector Size{};
  /// The IMAGEDEF that names the picture's file. Group 340.
  Handle Definition{};
  /// 1 shown, 2 shown when not aligned with the screen, 4 clipped, 8 transparent; 7 for a new one, as AutoCAD
  /// writes it. Group 70.
  int Flags{Displayed};
  /// Whether the clipping boundary is on. Group 280.
  int Clipped{};
  /// The brightness, 0 to 100. Group 281.
  int Brightness{Halfway};
  /// The contrast, 0 to 100. Group 282.
  int Contrast{Halfway};
  /// The fade, 0 to 100. Group 283.
  int Fade{};
  /// Whether the clip is inverted, hiding what lies inside the boundary. Group 290, from R2010.
  bool Outside{};
  /// The IMAGEDEF_REACTOR that ties the image to its definition. Group 360.
  Handle Reactor{};
  /// The shape of the clipping boundary: 1 a rectangle, as for a new one, 2 a polygon. Group 71.
  int Kind{1};
  /// The vertices of the clipping boundary, two opposite corners for a rectangle; group 91 counts them.
  std::vector<Pixel> Corners{};

  auto operator==(const Raster &) const -> bool = default;
};

/// An IMAGE: a raster picture placed in the drawing, its file named by an IMAGEDEF.
struct DXFIO_API Image
{
  /// The layer, linetype, color and the rest every entity carries.
  Entities::Record Record{};
  /// The proxy graphics, for programs that cannot draw the entity. Group 92, or 160 from R2010, then 310.
  std::string Picture{};
  /// Where the picture lies, how it is shown and how it is clipped, from the AcDbRasterImage subclass.
  Boxed<Entities::Raster> Raster{};

  auto operator==(const Image &) const -> bool = default;
};

/// A WIPEOUT: an area that hides what is drawn under it, laid out as an image.
struct DXFIO_API Wipeout
{
  /// The layer, linetype, color and the rest every entity carries.
  Entities::Record Record{};
  /// The proxy graphics, for programs that cannot draw the entity. Group 92, or 160 from R2010, then 310.
  std::string Picture{};
  /// Where the area lies and the boundary that shapes it, from the AcDbWipeout subclass.
  Boxed<Entities::Raster> Raster{};

  auto operator==(const Wipeout &) const -> bool = default;
};
} // namespace DxfIO::Entities

#endif /* DXFIO_ENTITIES_RASTER_HPP */
