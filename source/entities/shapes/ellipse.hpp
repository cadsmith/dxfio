#ifndef DXFIO_ENTITIES_SHAPES_ELLIPSE_HPP
#define DXFIO_ENTITIES_SHAPES_ELLIPSE_HPP

#include "fields/field.hpp"
#include "fields/numbers.hpp"
#include "fields/points.hpp"
#include "fields/subclass.hpp"
#include "record.hpp"
#include "shapes.hpp"

#include "dxfio/entities/ellipse.hpp"
#include "dxfio/vector.hpp"

#include <string_view>
#include <tuple>

namespace DxfIO
{
  template <>
  inline constexpr auto Shapes<Entities::Ellipse> = std::tuple{
      Entities::Part<Entities::Ellipse>,
      Fields::Subclass{.Marker = "AcDbEllipse"},
      Fields::Field{.Code = 10, .Member = &Entities::Ellipse::Center},
      Fields::Field{.Code = 11, .Member = &Entities::Ellipse::Axis},
      Fields::Field{.Code = 210, .Member = &Entities::Ellipse::Extrusion},
      Fields::Field{.Code = 40, .Member = &Entities::Ellipse::Ratio},
      Fields::Field{.Code = 41, .Member = &Entities::Ellipse::Start},
      Fields::Field{.Code = 42, .Member = &Entities::Ellipse::End},
  };

  template <>
  inline constexpr std::string_view Type<Entities::Ellipse> = "ELLIPSE";
} // namespace DxfIO

#endif /* DXFIO_ENTITIES_SHAPES_ELLIPSE_HPP */
