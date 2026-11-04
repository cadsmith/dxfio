#ifndef DXFIO_OBJECTS_MATERIAL_HPP
#define DXFIO_OBJECTS_MATERIAL_HPP

#include "dxfio/api.hpp"
#include "dxfio/base.hpp"
#include "dxfio/boxed.hpp"
#include "dxfio/matrix.hpp"

#include <optional>
#include <string>

namespace DxfIO::Objects
{
/// Black as AutoCAD packs a color into one integer: a true color of 0, 0, 0.
inline constexpr int Black = -1040187392;

/// A map source: the current scene.
inline constexpr int Scene = 0;

/// A map source: an image file.
inline constexpr int Bitmap = 1;

/// A map projection: onto the faces of a box.
inline constexpr int Box = 2;

/// A map tiling: the image repeats.
inline constexpr int Tiled = 1;

/// A map's automatic transform: none.
inline constexpr int Manual = 1;

/// The gloss of a new material.
inline constexpr double Glossy = 0.5;

/// One color of a material: its ambient, diffuse or specular color.
struct DXFIO_API Shade
{
  /// 0 to take the color of the object, 1 to override it with Color. Group 70, 71 or 76 by shade.
  int Method{};
  /// How much of the color shows, from 0 to 1. Group 40, 41 or 45 by shade.
  double Factor{1};
  /// The color, packed into one integer as AutoCAD does. Group 90, 91 or 92 by shade.
  int Color{Black};

  auto operator==(const Shade &) const -> bool = default;
};

/// One map of a material: the image laid over a channel and how it is projected. Its groups depend on the map;
/// each member gives them in the order diffuse, specular, reflection, opacity, bump, refraction.
struct DXFIO_API Map
{
  /// How strongly the map shows, from 0 to 1. Group 42, 46, 48, 141, 143 or 146.
  double Blend{1};
  /// Where the map comes from: 0 the current scene, 1 an image file. Group 72, 77, 171, 175, 179 or 273.
  int Source{Bitmap};
  /// The image file; empty for none. Group 3, 4, 6, 7, 8 or 9.
  std::string File;
  /// How the map is projected: 1 planar, 2 box, 3 cylinder, 4 sphere. Group 73, 78, 172, 176, 270 or 274.
  int Projection{Box};
  /// How the map fills the surface: 1 tile, 2 crop, 3 clamp. Group 74, 79, 173, 177, 271 or 275.
  int Tiling{Tiled};
  /// Bits: 1 for no automatic transform, 2 to fit the map to the object's extents, 4 to add the block's transform.
  /// Group 75, 170, 174, 178, 272 or 276.
  int Automatic{Manual};
  /// The transform of the map's projection, 16 values row by row. Group 43, 47, 49, 142, 144 or 147.
  DxfIO::Matrix Transform{};

  auto operator==(const Map &) const -> bool = default;
};

/// The maps of a material, one per channel.
struct DXFIO_API Maps
{
  /// The map over the diffuse color.
  Map Diffuse{};
  /// The map over the specular highlights.
  Map Specular{};
  /// The map of what the material reflects.
  Map Reflection{};
  /// The map of where the material is see-through.
  Map Opacity{};
  /// The map that makes the surface look raised.
  Map Bump{};
  /// The map of how light bends through the material; it takes the current scene unless told.
  Map Refraction{
      .Blend = 1,
      .Source = Scene,
      .File = {},
      .Projection = Box,
      .Tiling = Tiled,
      .Automatic = Manual,
      .Transform = Identity,
  };

  auto operator==(const Maps &) const -> bool = default;
};

/// A MATERIAL: how the surfaces of the objects it is given to are rendered.
struct DXFIO_API Material
{
  /// The handle, owner, reactors and extended data.
  DxfIO::Base Base{};
  /// The name of the material, such as "Global". Group 1.
  std::string Name;
  /// The material's description. Group 2.
  std::string Description;
  /// The color in ambient light. Groups 70, 40, 90.
  Shade Ambient{};
  /// The color under direct light. Groups 71, 41, 91.
  Shade Diffuse{};
  /// How glossy the surface is, from 0 to 1. Group 44.
  double Gloss{Glossy};
  /// The color of highlights. Groups 76, 45, 92.
  Shade Specular{};
  /// How opaque the material is, from 0 to 1. Group 140.
  double Opacity{1};
  /// The index of refraction. Group 145.
  double Refraction{1};
  /// The maps laid over its channels, kept on the heap: six maps would make every object as large.
  Boxed<Objects::Maps> Maps;
  /// How much light passes through the material, from 0 to 1. Group 148, from R2007.
  double Translucence{};
  /// How much light the material gives off itself. Group 149, from R2007.
  double Illumination{};
  /// How much light the material reflects. Group 468, from R2007.
  double Reflectivity{};
  /// The illumination model it is shaded with. Group 93, from R2007.
  int Model{};
  /// Bits for the channels in use, one per map in the order of Maps; 63 for all six. Group 94, from R2007.
  std::optional<int> Channels;
  /// The material's rendering mode. Group 282, from R2007.
  int Mode{};

  auto operator==(const Material &) const -> bool = default;
};
} // namespace DxfIO::Objects

#endif /* DXFIO_OBJECTS_MATERIAL_HPP */
