#ifndef DXFIO_OBJECTS_IMAGE_DEFINITION_HPP
#define DXFIO_OBJECTS_IMAGE_DEFINITION_HPP

#include "dxfio/api.hpp"
#include "dxfio/base.hpp"
#include "dxfio/vector.hpp"

#include <string>

namespace DxfIO::Objects
{
/// An IMAGEDEF: a raster image file the drawing's IMAGE entities show.
struct DXFIO_API ImageDef
{
  /// The handle, owner, reactors and extended data.
  DxfIO::Base Base{};
  /// The version of the object, 0 as AutoCAD writes it. Group 90.
  int Version{};
  /// The path of the image file. Group 1.
  std::string File;
  /// The width and height of the image, in pixels. Groups 10, 20.
  Vector Size{.X = 1, .Y = 1};
  /// The width and height of one pixel, in drawing units. Groups 11, 21.
  Vector Pixel{};
  /// 1 when the image is loaded, 0 when not. Group 280.
  int Loaded{};
  /// The units Pixel is in: 0 none, 2 centimeters, 5 inches. Group 281.
  int Units{};

  auto operator==(const ImageDef &) const -> bool = default;
};

/// An IMAGEDEF_REACTOR: the link from an image definition back to an IMAGE entity that shows it.
struct DXFIO_API ImageDefReactor
{
  /// The handle, owner, reactors and extended data.
  DxfIO::Base Base{};
  /// The version of the object, 2 as AutoCAD writes it. Group 90.
  int Version{2};
  /// The IMAGE entity. Group 330.
  Handle Image{};

  auto operator==(const ImageDefReactor &) const -> bool = default;
};

/// A RASTERVARIABLES: how the drawing shows its raster images.
struct DXFIO_API RasterVariables
{
  /// The handle, owner, reactors and extended data.
  DxfIO::Base Base{};
  /// The version of the object, 0 as AutoCAD writes it. Group 90.
  int Version{};
  /// 1 to draw a frame around images, 0 not to. Group 70.
  int Frame{};
  /// The display quality: 0 draft, 1 high. Group 71.
  int Quality{};
  /// The units images are inserted in: 0 none, 1 millimeters, 2 centimeters, 3 meters, 4 kilometers, 5 inches,
  /// 6 feet, 7 yards, 8 miles. Group 72.
  int Units{};

  auto operator==(const RasterVariables &) const -> bool = default;
};
} // namespace DxfIO::Objects

#endif /* DXFIO_OBJECTS_IMAGE_DEFINITION_HPP */
