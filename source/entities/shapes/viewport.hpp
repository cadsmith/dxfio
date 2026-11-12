#ifndef DXFIO_ENTITIES_SHAPES_VIEWPORT_HPP
#define DXFIO_ENTITIES_SHAPES_VIEWPORT_HPP

#include "../../tables/shapes/axes.hpp"
#include "fields/custom.hpp"
#include "fields/field.hpp"
#include "fields/flat.hpp"
#include "fields/many.hpp"
#include "fields/numbers.hpp"
#include "fields/optional.hpp"
#include "fields/part.hpp"
#include "fields/points.hpp"
#include "fields/since.hpp"
#include "fields/subclass.hpp"
#include "fields/text.hpp"
#include "record.hpp"
#include "shapes.hpp"

#include "dxfio/entities/viewport.hpp"
#include "dxfio/version.hpp"

#include <expected>
#include <string_view>
#include <tuple>

namespace DxfIO
{
  namespace Entities
  {
    inline constexpr Fields::Many Legacy{.Code = 341, .Member = &Port::Frozen};
    inline constexpr Fields::Many Modern{.Code = 331, .Member = &Port::Frozen};

    inline void Frozen(Port &one, Fields::Walk &in)
    {
      Fields::Take(Legacy, one, in);
      Fields::Take(Modern, one, in);
    }

    inline auto Freezing(const Port &one, Fields::Writing &to) -> std::expected<void, Failure>
    {
      return to.Release < Version::R2004 ? Fields::Put(Legacy, one, to)
                                         : Fields::Put(Modern, one, to);
    }

    inline constexpr auto Views = std::tuple{
        Fields::Flat{.Code = 12, .Member = &Port::View},
        Fields::Flat{.Code = 13, .Member = &Port::Base},
        Fields::Flat{.Code = 14, .Member = &Port::Snap},
        Fields::Flat{.Code = 15, .Member = &Port::Grid},
        Fields::Field{.Code = 16, .Member = &Port::Direction},
        Fields::Field{.Code = 17, .Member = &Port::Target},
        Fields::Field{.Code = 42, .Member = &Port::Lens},
        Fields::Field{.Code = 43, .Member = &Port::Front},
        Fields::Field{.Code = 44, .Member = &Port::Back},
        Fields::Field{.Code = 45, .Member = &Port::Extent},
        Fields::Field{.Code = 50, .Member = &Port::Angle},
        Fields::Field{.Code = 51, .Member = &Port::Twist},
        Fields::Field{.Code = 72, .Member = &Port::Zoom},
        Fields::Custom<Port>{.Read = Frozen, .Write = Freezing},
        Fields::Field{.Code = 90, .Member = &Port::Flags},
        Fields::Field{.Code = 340, .Member = &Port::Clip, .Fallback = Handle{}},
        Fields::Field{.Code = 1, .Member = &Port::Sheet},
        Fields::Field{.Code = 281, .Member = &Port::Render},
        Fields::Field{.Code = 71, .Member = &Port::Follows},
        Fields::Field{.Code = 74, .Member = &Port::Icon},
        Fields::Part{.Member = &Port::Axes, .Fields = Shapes<Tables::Axes>},
    };

    inline constexpr auto Lights = std::tuple{
        Fields::Field{.Code = 61, .Member = &Port::Major},
        Fields::Field{.Code = 332, .Member = &Port::Background, .Fallback = Handle{}},
        Fields::Field{.Code = 333, .Member = &Port::Plot, .Fallback = Handle{}},
        Fields::Field{.Code = 348, .Member = &Port::Style, .Fallback = Handle{}},
        Fields::Field{.Code = 292, .Member = &Port::Lit},
        Fields::Field{.Code = 282, .Member = &Port::Lighting},
        Fields::Field{.Code = 141, .Member = &Port::Brightness},
        Fields::Field{.Code = 142, .Member = &Port::Contrast},
        Fields::Field{.Code = 63, .Member = &Port::Ambient},
        Fields::Field{.Code = 421, .Member = &Port::Rgb},
        Fields::Field{
            .Code     = 431,
            .Member   = &Port::Book,
            .Fallback = std::string_view{},
        },
        Fields::Field{.Code = 361, .Member = &Port::Sun, .Fallback = Handle{}},
    };
  } // namespace Entities

  template <>
  inline constexpr auto Shapes<Entities::Port> = std::tuple{
      Fields::Subclass{.Marker = "AcDbViewport"},
      Fields::Field{.Code = 10, .Member = &Entities::Port::Center},
      Fields::Field{.Code = 40, .Member = &Entities::Port::Width},
      Fields::Field{.Code = 41, .Member = &Entities::Port::Height},
      Fields::Field{.Code = 68, .Member = &Entities::Port::Status},
      Fields::Field{.Code = 69, .Member = &Entities::Port::Id},
      Fields::Since{.Release = Version::R2000, .Fields = Entities::Views},
      Fields::Since{
          .Release = Version::R2004,
          .Fields  = std::tuple{Fields::Field{
              .Code   = 170,
              .Member = &Entities::Port::Shade,
          }},
      },
      Fields::Since{.Release = Version::R2007, .Fields = Entities::Lights},
  };

  template <>
  inline constexpr auto Shapes<Entities::Viewport> = std::tuple{
      Entities::Part<Entities::Viewport>,
      Fields::Part{
          .Member = &Entities::Viewport::Port,
          .Fields = Shapes<Entities::Port>,
      },
  };

  template <>
  inline constexpr std::string_view Type<Entities::Viewport> = "VIEWPORT";
} // namespace DxfIO

#endif /* DXFIO_ENTITIES_SHAPES_VIEWPORT_HPP */
