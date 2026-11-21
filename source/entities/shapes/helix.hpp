#ifndef DXFIO_ENTITIES_SHAPES_HELIX_HPP
#define DXFIO_ENTITIES_SHAPES_HELIX_HPP

#include "fields/field.hpp"
#include "fields/numbers.hpp"
#include "fields/part.hpp"
#include "fields/picture.hpp"
#include "fields/points.hpp"
#include "fields/subclass.hpp"
#include "record.hpp"
#include "shapes.hpp"
#include "spline.hpp"

#include "dxfio/entities/helix.hpp"

#include <string_view>
#include <tuple>

namespace DxfIO
{
  template <>
  inline constexpr auto Shapes<Entities::HelixData> = std::tuple{
      Fields::Subclass{.Marker = "AcDbHelix"},
      Fields::Field{.Code = 90, .Member = &Entities::HelixData::Major},
      Fields::Field{.Code = 91, .Member = &Entities::HelixData::Minor},
      Fields::Field{.Code = 10, .Member = &Entities::HelixData::BasePoint},
      Fields::Field{.Code = 11, .Member = &Entities::HelixData::Start},
      Fields::Field{.Code = 12, .Member = &Entities::HelixData::Axis},
      Fields::Field{.Code = 40, .Member = &Entities::HelixData::Radius},
      Fields::Field{.Code = 41, .Member = &Entities::HelixData::Turns},
      Fields::Field{.Code = 42, .Member = &Entities::HelixData::Rise},
      Fields::Field{.Code = 290, .Member = &Entities::HelixData::RightHanded},
      Fields::Field{.Code = 280, .Member = &Entities::HelixData::Constraint},
  };

  template <>
  inline constexpr auto Shapes<Entities::Helix> = std::tuple{
      Entities::Part<Entities::Helix>,
      Fields::Picture{.Member = &Entities::Helix::Picture},
      Fields::Part{
          .Member = &Entities::Helix::Curve,
          .Fields = Shapes<Entities::Curve>,
      },
      Fields::Part{
          .Member = &Entities::Helix::Data,
          .Fields = Shapes<Entities::HelixData>,
      },
  };

  template <>
  inline constexpr std::string_view Type<Entities::Helix> = "HELIX";
} // namespace DxfIO

#endif /* DXFIO_ENTITIES_SHAPES_HELIX_HPP */
