#ifndef DXFIO_ENTITIES_SHAPES_UNDERLAY_HPP
#define DXFIO_ENTITIES_SHAPES_UNDERLAY_HPP

#include "fields/every.hpp"
#include "fields/field.hpp"
#include "fields/flat.hpp"
#include "fields/handles.hpp"
#include "fields/numbers.hpp"
#include "fields/part.hpp"
#include "fields/picture.hpp"
#include "fields/points.hpp"
#include "fields/subclass.hpp"
#include "insert.hpp"
#include "record.hpp"
#include "shapes.hpp"

#include "dxfio/entities/underlay.hpp"

#include <string_view>
#include <tuple>

namespace DxfIO
{
  template <>
  inline constexpr auto Shapes<Entities::Bound> = std::tuple{
      Fields::Flat{.Code = 11, .Member = &Entities::Bound::Position},
  };

  template <>
  inline constexpr auto Shapes<Entities::Overlaid> = std::tuple{
      Fields::Subclass{.Marker = "AcDbUnderlayReference"},
      Fields::Field{.Code = 340, .Member = &Entities::Overlaid::Definition},
      Fields::Field{.Code = 10, .Member = &Entities::Overlaid::Position},
      Fields::Part{
          .Member = &Entities::Overlaid::Scale,
          .Fields = Entities::Scales,
      },
      Fields::Field{
          .Code     = 50,
          .Member   = &Entities::Overlaid::Rotation,
          .Fallback = 0.0,
      },
      Fields::Field{
          .Code     = 210,
          .Member   = &Entities::Overlaid::Extrusion,
          .Fallback = Up,
      },
      Fields::Field{.Code = 280, .Member = &Entities::Overlaid::Flags},
      Fields::Field{.Code = 281, .Member = &Entities::Overlaid::Contrast},
      Fields::Field{.Code = 282, .Member = &Entities::Overlaid::Fade},
      Fields::Every{
          .Opener = 11,
          .Member = &Entities::Overlaid::Clip,
          .Fields = Shapes<Entities::Bound>,
      },
  };

  template <Entities::Overlay KindV>
  inline constexpr auto Shapes<Entities::UnderlayOf<KindV>> = std::tuple{
      Entities::Part<Entities::UnderlayOf<KindV>>,
      Fields::Picture{.Member = &Entities::UnderlayOf<KindV>::Picture},
      Fields::Part{
          .Member = &Entities::UnderlayOf<KindV>::Overlaid,
          .Fields = Shapes<Entities::Overlaid>,
      },
  };

  template <>
  inline constexpr std::string_view Type<Entities::PdfUnderlay> = "PDFUNDERLAY";

  template <>
  inline constexpr std::string_view Type<Entities::DwfUnderlay> = "DWFUNDERLAY";

  template <>
  inline constexpr std::string_view Type<Entities::DgnUnderlay> = "DGNUNDERLAY";
} // namespace DxfIO

#endif /* DXFIO_ENTITIES_SHAPES_UNDERLAY_HPP */
