#ifndef DXFIO_ENTITIES_LIGHT_HPP
#define DXFIO_ENTITIES_LIGHT_HPP

#include "dxfio/api.hpp"
#include "dxfio/boxed.hpp"
#include "dxfio/entities/record.hpp"
#include "dxfio/vector.hpp"

#include <string>

namespace DxfIO::Entities
{
  /**
   *  The true color a new light takes: white, as 0xRRGGBB. Not a color index: Colors::White is
   *  that.
   */
  inline constexpr int Lamplight = 0xFFFFFF;

  /**
   *  The AcDbLight subclass of a LIGHT: its kind, color and place, and how it falls off and casts
   *  shadows.
   */
  struct DXFIO_API Lamp
  {
    /**
     *  The version of the light's layout. Group 90.
     */
    int Version{1};
    /**
     *  The name of the light. Group 1.
     */
    std::string Name{};
    /**
     *  1 distant, 2 point, 3 spot. Group 70.
     */
    int Kind{2};
    /**
     *  Whether the light is on. Group 290.
     */
    bool Lit{true};
    /**
     *  The AutoCAD Color Index of the light. Group 63, from R2004.
     */
    int Color{7};
    /**
     *  The true color of the light, as 0xRRGGBB. Group 421 from R2004; up to R2000, packed in a
     *  group 90.
     */
    int Rgb{Lamplight};
    /**
     *  Whether the light's glyph is plotted. Group 291.
     */
    bool Glyph{};
    /**
     *  How bright the light is. Group 40.
     */
    double Intensity{1};
    /**
     *  The position, in world coordinates. Groups 10, 20, 30.
     */
    Vector Position{};
    /**
     *  The point a spot or distant light aims at, in world coordinates. Groups 11, 21, 31.
     */
    Vector Target{};
    /**
     *  How the light weakens with distance: 0 not at all, 1 inverse linear, 2 inverse square. Group
     *  72.
     */
    int Fading{};
    /**
     *  Whether the light reaches only from its start distance to its end distance. Group 292.
     */
    bool Limited{};
    /**
     *  The distance the light starts to reach from, when it is limited. Group 41.
     */
    double Start{1};
    /**
     *  The distance the light stops reaching at, when it is limited. Group 42.
     */
    double End{10};
    /**
     *  The angle of a spot light's bright inner cone, in degrees. Group 50.
     */
    double Hotspot{45};
    /**
     *  The angle of a spot light's whole cone, in degrees. Group 51.
     */
    double Falloff{50};
    /**
     *  Whether the light casts shadows. Group 293.
     */
    bool Casting{true};
    /**
     *  The kind of shadow: 0 ray traced, 1 shadow map. Group 73.
     */
    int Shading{};
    /**
     *  The size of the shadow map. Group 91.
     */
    int Size{256};
    /**
     *  The softness of the shadow map's edges. Group 280.
     */
    int Softness{1};

    auto operator==(const Lamp &) const -> bool = default;
  };

  /**
   *  A LIGHT: a point, spot or distant light of the scene.
   */
  struct DXFIO_API Light
  {
    /**
     *  The layer, linetype, color and the rest every entity carries.
     */
    Entities::Record Record{};
    /**
     *  The proxy graphics, for programs that cannot draw the entity. Group 92, or 160 from R2010,
     *  then 310.
     */
    std::string Picture{};
    /**
     *  The kind, color and place of the light, from the AcDbLight subclass.
     */
    Boxed<Entities::Lamp> Lamp{};

    auto operator==(const Light &) const -> bool = default;
  };
} // namespace DxfIO::Entities

#endif /* DXFIO_ENTITIES_LIGHT_HPP */
