#ifndef DXFIO_ENTITIES_SHAPES_POINT_HPP
#define DXFIO_ENTITIES_SHAPES_POINT_HPP

#include "fields/field.hpp"
#include "fields/numbers.hpp"
#include "fields/points.hpp"
#include "fields/subclass.hpp"
#include "record.hpp"
#include "shapes.hpp"

#include "dxfio/entities/point.hpp"
#include "dxfio/vector.hpp"

#include <string_view>
#include <tuple>

namespace DxfIO
{
  template <>
  inline constexpr auto Shapes<Entities::Point> = std::tuple{
      Entities::Part<Entities::Point>,
      Fields::Subclass{.Marker = "AcDbPoint"},
      Fields::Field{.Code = 10, .Member = &Entities::Point::Position},
      Fields::Field{
          .Code     = 39,
          .Member   = &Entities::Point::Thickness,
          .Fallback = 0.0,
      },
      Fields::Field{
          .Code     = 210,
          .Member   = &Entities::Point::Extrusion,
          .Fallback = Up,
      },
      Fields::Field{
          .Code     = 50,
          .Member   = &Entities::Point::Angle,
          .Fallback = 0.0,
      },
  };

  template <>
  inline constexpr std::string_view Type<Entities::Point> = "POINT";
} // namespace DxfIO

#endif /* DXFIO_ENTITIES_SHAPES_POINT_HPP */
