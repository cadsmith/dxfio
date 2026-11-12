#ifndef DXFIO_ENTITIES_SHAPES_LIGHT_HPP
#define DXFIO_ENTITIES_SHAPES_LIGHT_HPP

#include "fields/custom.hpp"
#include "fields/field.hpp"
#include "fields/numbers.hpp"
#include "fields/part.hpp"
#include "fields/picture.hpp"
#include "fields/points.hpp"
#include "fields/sequence.hpp"
#include "fields/since.hpp"
#include "fields/text.hpp"
#include "fields/until.hpp"
#include "record.hpp"
#include "shapes.hpp"

#include "dxfio/entities/light.hpp"
#include "dxfio/version.hpp"

#include <cstdint>
#include <expected>
#include <optional>
#include <string_view>
#include <tuple>

namespace DxfIO
{
  namespace Entities
  {
    inline constexpr int Packed = 90;

    inline constexpr std::uint32_t Method = 0xC2000000U;

    inline constexpr std::uint32_t Colors = 0x00FFFFFFU;

    inline void Unpacked(Lamp &one, Fields::Walk &in)
    {
      if (const std::optional<int> held = Fields::Value<int>::Read(in, Packed))
      {
        one.Rgb = static_cast<int>(static_cast<std::uint32_t>(*held) & Colors);
      }
    }

    inline auto Packing(const Lamp &one, Fields::Writing &to) -> std::expected<void, Failure>
    {
      const std::uint32_t held = Method | (static_cast<std::uint32_t>(one.Rgb) & Colors);
      return Fields::Value<int>::Write(to, Packed, static_cast<int>(held));
    }
  } // namespace Entities

  template <>
  inline constexpr auto Shapes<Entities::Lamp> = std::tuple{
      Fields::Sequence{.Marker = "AcDbLight"},
      Fields::Field{.Code = 90, .Member = &Entities::Lamp::Version},
      Fields::Field{.Code = 1, .Member = &Entities::Lamp::Name},
      Fields::Field{.Code = 70, .Member = &Entities::Lamp::Kind},
      Fields::Field{.Code = 290, .Member = &Entities::Lamp::Lit},
      Fields::Until{
          .Release = Version::R2000,
          .Fields =
              std::tuple{
                  Fields::Custom<Entities::Lamp>{
                      .Read  = Entities::Unpacked,
                      .Write = Entities::Packing,
                  },
              },
      },
      Fields::Since{
          .Release = Version::R2004,
          .Fields =
              std::tuple{
                  Fields::Field{.Code = 63, .Member = &Entities::Lamp::Color},
                  Fields::Field{.Code = 421, .Member = &Entities::Lamp::Rgb},
              },
      },
      Fields::Field{.Code = 291, .Member = &Entities::Lamp::Glyph},
      Fields::Field{.Code = 40, .Member = &Entities::Lamp::Intensity},
      Fields::Field{.Code = 10, .Member = &Entities::Lamp::Position},
      Fields::Field{.Code = 11, .Member = &Entities::Lamp::Target},
      Fields::Field{.Code = 72, .Member = &Entities::Lamp::Fading},
      Fields::Field{.Code = 292, .Member = &Entities::Lamp::Limited},
      Fields::Field{.Code = 41, .Member = &Entities::Lamp::Start},
      Fields::Field{.Code = 42, .Member = &Entities::Lamp::End},
      Fields::Field{.Code = 50, .Member = &Entities::Lamp::Hotspot},
      Fields::Field{.Code = 51, .Member = &Entities::Lamp::Falloff},
      Fields::Field{.Code = 293, .Member = &Entities::Lamp::Casting},
      Fields::Field{.Code = 73, .Member = &Entities::Lamp::Shading},
      Fields::Field{.Code = 91, .Member = &Entities::Lamp::Size},
      Fields::Field{.Code = 280, .Member = &Entities::Lamp::Softness},
  };

  template <>
  inline constexpr auto Shapes<Entities::Light> = std::tuple{
      Entities::Part<Entities::Light>,
      Fields::Picture{.Member = &Entities::Light::Picture},
      Fields::Part{
          .Member = &Entities::Light::Lamp,
          .Fields = Shapes<Entities::Lamp>,
      },
  };

  template <>
  inline constexpr std::string_view Type<Entities::Light> = "LIGHT";
} // namespace DxfIO

#endif /* DXFIO_ENTITIES_SHAPES_LIGHT_HPP */
