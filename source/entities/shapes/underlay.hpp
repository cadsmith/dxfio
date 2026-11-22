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
  inline constexpr auto Shapes<Entities::UnderlayData> = std::tuple{
      Fields::Subclass{.Marker = "AcDbUnderlayReference"},
      Fields::Field{.Code = 340, .Member = &Entities::UnderlayData::Definition},
      Fields::Field{.Code = 10, .Member = &Entities::UnderlayData::Position},
      Fields::Part{
          .Member = &Entities::UnderlayData::Scale,
          .Fields = Entities::Scales,
      },
      Fields::Field{
          .Code     = 50,
          .Member   = &Entities::UnderlayData::Rotation,
          .Fallback = 0.0,
      },
      Fields::Field{
          .Code     = 210,
          .Member   = &Entities::UnderlayData::Extrusion,
          .Fallback = Up,
      },
      Fields::Field{.Code = 280, .Member = &Entities::UnderlayData::Flags},
      Fields::Field{.Code = 281, .Member = &Entities::UnderlayData::Contrast},
      Fields::Field{.Code = 282, .Member = &Entities::UnderlayData::Fade},
      Fields::Every{
          .Opener = 11,
          .Member = &Entities::UnderlayData::Clip,
          .Fields = Shapes<Entities::Bound>,
      },
  };

  template <Entities::UnderlayKind KindV>
  inline constexpr auto Shapes<Entities::UnderlayOf<KindV>> = std::tuple{
      Entities::Part<Entities::UnderlayOf<KindV>>,
      Fields::Picture{.Member = &Entities::UnderlayOf<KindV>::Picture},
      Fields::Part{
          .Member = &Entities::UnderlayOf<KindV>::Data,
          .Fields = Shapes<Entities::UnderlayData>,
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
