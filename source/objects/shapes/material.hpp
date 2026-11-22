#ifndef DXFIO_OBJECTS_SHAPES_MATERIAL_HPP
#define DXFIO_OBJECTS_SHAPES_MATERIAL_HPP

#include "fields/field.hpp"
#include "fields/matrices.hpp"
#include "fields/numbers.hpp"
#include "fields/optional.hpp"
#include "fields/part.hpp"
#include "fields/since.hpp"
#include "fields/subclass.hpp"
#include "fields/text.hpp"
#include "shapes.hpp"

#include "dxfio/matrix.hpp"
#include "dxfio/objects/material.hpp"
#include "dxfio/version.hpp"

#include <optional>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>

namespace DxfIO
{
  namespace Objects
  {
    struct Codes
    {
      int Blend{};
      int Source{};
      int File{};
      int Projection{};
      int Tiling{};
      int Automatic{};
      int Transform{};
      int Fallback{1};
    };

    template <int Method, int Factor, int Color>
    inline constexpr auto Shaded = std::tuple{
        Fields::Field<Materials::MaterialColor, int, int>{
            .Code     = Method,
            .Member   = &Materials::MaterialColor::Method,
            .Fallback = 0,
        },
        Fields::Field<Materials::MaterialColor, double, double>{
            .Code     = Factor,
            .Member   = &Materials::MaterialColor::Factor,
            .Fallback = 1.0,
        },
        Fields::Field<Materials::MaterialColor, int, int>{
            .Code     = Color,
            .Member   = &Materials::MaterialColor::Color,
            .Fallback = -1040187392,
        },
    };

    template <Codes At>
    inline constexpr auto Channel = std::tuple{
        Fields::Field<Materials::Map, double, double>{
            .Code     = At.Blend,
            .Member   = &Materials::Map::Blend,
            .Fallback = 1.0,
        },
        Fields::Field<Materials::Map, int, int>{
            .Code     = At.Source,
            .Member   = &Materials::Map::Source,
            .Fallback = At.Fallback,
        },
        Fields::Field<Materials::Map, std::string, std::string_view>{
            .Code     = At.File,
            .Member   = &Materials::Map::File,
            .Fallback = std::string_view{},
        },
        Fields::Field<Materials::Map, int, int>{
            .Code     = At.Projection,
            .Member   = &Materials::Map::Projection,
            .Fallback = 2,
        },
        Fields::Field<Materials::Map, int, int>{
            .Code     = At.Tiling,
            .Member   = &Materials::Map::Tiling,
            .Fallback = 1,
        },
        Fields::Field<Materials::Map, int, int>{
            .Code     = At.Automatic,
            .Member   = &Materials::Map::Automatic,
            .Fallback = 1,
        },
        Fields::Field<Materials::Map, Matrix, Matrix>{
            .Code     = At.Transform,
            .Member   = &Materials::Map::Transform,
            .Fallback = Identity,
        },
    };

    template <Materials::Map Materials::Maps::*Member, Codes At>
    inline constexpr Fields::Part<Materials::Maps, Materials::Map,
                                  std::remove_const_t<decltype(Channel<At>)>>
        Placed{
            .Member = Member,
            .Fields = Channel<At>,
    };

    template <Materials::Map Materials::Maps::*Member, Codes At>
    inline constexpr Fields::Part<Material, Boxed<Materials::Maps>,
                                  std::tuple<std::remove_const_t<decltype(Placed<Member, At>)>>>
        Mapped{.Member = &Material::Maps, .Fields = std::tuple{Placed<Member, At>}};

    inline constexpr auto Added = std::tuple{
        Fields::Field{
            .Code     = 148,
            .Member   = &Material::Translucence,
            .Fallback = 0.0,
        },
        Fields::Field{
            .Code     = 149,
            .Member   = &Material::Illumination,
            .Fallback = 0.0,
        },
        Fields::Field{
            .Code     = 468,
            .Member   = &Material::Reflectivity,
            .Fallback = 0.0,
        },
        Fields::Field{.Code = 93, .Member = &Material::Model, .Fallback = 0},
        Fields::Field{
            .Code     = 94,
            .Member   = &Material::Channels,
            .Fallback = Fields::Empty{},
        },
        Fields::Field{.Code = 282, .Member = &Material::Mode, .Fallback = 0},
    };
  } // namespace Objects

  template <>
  inline constexpr auto Shapes<Objects::Material> = std::tuple{
      Fields::Subclass{.Marker = "AcDbMaterial"},
      Fields::Field{.Code = 1, .Member = &Objects::Material::Name},
      Fields::Field{
          .Code     = 2,
          .Member   = &Objects::Material::Description,
          .Fallback = std::string_view{},
      },
      Fields::Part{
          .Member = &Objects::Material::Ambient,
          .Fields = Objects::Shaded<70, 40, 90>,
      },
      Fields::Part{
          .Member = &Objects::Material::Diffuse,
          .Fields = Objects::Shaded<71, 41, 91>,
      },
      Objects::Mapped<&Objects::Materials::Maps::Diffuse,
                      Objects::Codes{
                          .Blend      = 42,
                          .Source     = 72,
                          .File       =  3,
                          .Projection = 73,
                          .Tiling     = 74,
                          .Automatic  = 75,
                          .Transform  = 43,
                      }>,
      Fields::Field{
          .Code     = 44,
          .Member   = &Objects::Material::Gloss,
          .Fallback = 0.5,
      },
      Fields::Part{
          .Member = &Objects::Material::Specular,
          .Fields = Objects::Shaded<76, 45, 92>,
      },
      Objects::Mapped<&Objects::Materials::Maps::Specular,
                      Objects::Codes{
                          .Blend      =  46,
                          .Source     =  77,
                          .File       =   4,
                          .Projection =  78,
                          .Tiling     =  79,
                          .Automatic  = 170,
                          .Transform  =  47,
                      }>,
      Objects::Mapped<&Objects::Materials::Maps::Reflection,
                      Objects::Codes{
                          .Blend      =  48,
                          .Source     = 171,
                          .File       =   6,
                          .Projection = 172,
                          .Tiling     = 173,
                          .Automatic  = 174,
                          .Transform  =  49,
                      }>,
      Fields::Field{
          .Code     = 140,
          .Member   = &Objects::Material::Opacity,
          .Fallback = 1.0,
      },
      Objects::Mapped<&Objects::Materials::Maps::Opacity,
                      Objects::Codes{
                          .Blend      = 141,
                          .Source     = 175,
                          .File       =   7,
                          .Projection = 176,
                          .Tiling     = 177,
                          .Automatic  = 178,
                          .Transform  = 142,
                      }>,
      Objects::Mapped<&Objects::Materials::Maps::Bump,
                      Objects::Codes{
                          .Blend      = 143,
                          .Source     = 179,
                          .File       =   8,
                          .Projection = 270,
                          .Tiling     = 271,
                          .Automatic  = 272,
                          .Transform  = 144,
                      }>,
      Fields::Field{
          .Code     = 145,
          .Member   = &Objects::Material::Refraction,
          .Fallback = 1.0,
      },
      Objects::Mapped<&Objects::Materials::Maps::Refraction,
                      Objects::Codes{
                          .Blend      = 146,
                          .Source     = 273,
                          .File       =   9,
                          .Projection = 274,
                          .Tiling     = 275,
                          .Automatic  = 276,
                          .Transform  = 147,
                          .Fallback   =   0,
                      }>,
      Fields::Since{.Release = Version::R2007, .Fields = Objects::Added},
  };

  template <>
  inline constexpr std::string_view Type<Objects::Material> = "MATERIAL";
} // namespace DxfIO

#endif /* DXFIO_OBJECTS_SHAPES_MATERIAL_HPP */
