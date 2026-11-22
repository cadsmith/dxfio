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
    inline constexpr Fields::Many Legacy{.Code = 341, .Member = &ViewportData::Frozen};
    inline constexpr Fields::Many Modern{.Code = 331, .Member = &ViewportData::Frozen};

    inline void Frozen(ViewportData &one, Fields::Walk &in)
    {
      Fields::Take(Legacy, one, in);
      Fields::Take(Modern, one, in);
    }

    inline auto Freezing(const ViewportData &one, Fields::Writing &to)
        -> std::expected<void, Failure>
    {
      return to.Release < Version::R2004 ? Fields::Put(Legacy, one, to)
                                         : Fields::Put(Modern, one, to);
    }

    inline constexpr auto Views = std::tuple{
        Fields::Flat{.Code = 12, .Member = &ViewportData::ViewCenter},
        Fields::Flat{.Code = 13, .Member = &ViewportData::SnapBase},
        Fields::Flat{.Code = 14, .Member = &ViewportData::Snap},
        Fields::Flat{.Code = 15, .Member = &ViewportData::Grid},
        Fields::Field{.Code = 16, .Member = &ViewportData::Direction},
        Fields::Field{.Code = 17, .Member = &ViewportData::Target},
        Fields::Field{.Code = 42, .Member = &ViewportData::LensLength},
        Fields::Field{.Code = 43, .Member = &ViewportData::Front},
        Fields::Field{.Code = 44, .Member = &ViewportData::Back},
        Fields::Field{.Code = 45, .Member = &ViewportData::ViewHeight},
        Fields::Field{.Code = 50, .Member = &ViewportData::SnapAngle},
        Fields::Field{.Code = 51, .Member = &ViewportData::Twist},
        Fields::Field{.Code = 72, .Member = &ViewportData::Zoom},
        Fields::Custom<ViewportData>{.Read = Frozen, .Write = Freezing},
        Fields::Field{.Code = 90, .Member = &ViewportData::Flags},
        Fields::Field{.Code = 340, .Member = &ViewportData::Clip, .Fallback = Handle{}},
        Fields::Field{.Code = 1, .Member = &ViewportData::Sheet},
        Fields::Field{.Code = 281, .Member = &ViewportData::Render},
        Fields::Field{.Code = 71, .Member = &ViewportData::UcsPerViewport},
        Fields::Field{.Code = 74, .Member = &ViewportData::UcsIconAtOrigin},
        Fields::Part{.Member = &ViewportData::Axes, .Fields = Shapes<Tables::Axes>},
    };

    inline constexpr auto Lights = std::tuple{
        Fields::Field{.Code = 61, .Member = &ViewportData::GridMajor},
        Fields::Field{.Code = 332, .Member = &ViewportData::Background, .Fallback = Handle{}},
        Fields::Field{.Code = 333, .Member = &ViewportData::ShadePlotObject, .Fallback = Handle{}},
        Fields::Field{.Code = 348, .Member = &ViewportData::VisualStyle, .Fallback = Handle{}},
        Fields::Field{.Code = 292, .Member = &ViewportData::DefaultLighting},
        Fields::Field{.Code = 282, .Member = &ViewportData::DefaultLightingType},
        Fields::Field{.Code = 141, .Member = &ViewportData::Brightness},
        Fields::Field{.Code = 142, .Member = &ViewportData::Contrast},
        Fields::Field{.Code = 63, .Member = &ViewportData::Ambient},
        Fields::Field{.Code = 421, .Member = &ViewportData::Rgb},
        Fields::Field{
            .Code     = 431,
            .Member   = &ViewportData::Book,
            .Fallback = std::string_view{},
        },
        Fields::Field{.Code = 361, .Member = &ViewportData::Sun, .Fallback = Handle{}},
    };
  } // namespace Entities

  template <>
  inline constexpr auto Shapes<Entities::ViewportData> = std::tuple{
      Fields::Subclass{.Marker = "AcDbViewport"},
      Fields::Field{.Code = 10, .Member = &Entities::ViewportData::Center},
      Fields::Field{.Code = 40, .Member = &Entities::ViewportData::Width},
      Fields::Field{.Code = 41, .Member = &Entities::ViewportData::Height},
      Fields::Field{.Code = 68, .Member = &Entities::ViewportData::Status},
      Fields::Field{.Code = 69, .Member = &Entities::ViewportData::Id},
      Fields::Since{.Release = Version::R2000, .Fields = Entities::Views},
      Fields::Since{
          .Release = Version::R2004,
          .Fields  = std::tuple{Fields::Field{
              .Code   = 170,
              .Member = &Entities::ViewportData::ShadePlotMode,
          }},
      },
      Fields::Since{.Release = Version::R2007, .Fields = Entities::Lights},
  };

  template <>
  inline constexpr auto Shapes<Entities::Viewport> = std::tuple{
      Entities::Part<Entities::Viewport>,
      Fields::Part{
          .Member = &Entities::Viewport::Data,
          .Fields = Shapes<Entities::ViewportData>,
      },
  };

  template <>
  inline constexpr std::string_view Type<Entities::Viewport> = "VIEWPORT";
} // namespace DxfIO

#endif /* DXFIO_ENTITIES_SHAPES_VIEWPORT_HPP */
