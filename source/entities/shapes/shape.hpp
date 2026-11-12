#ifndef DXFIO_ENTITIES_SHAPES_SHAPE_HPP
#define DXFIO_ENTITIES_SHAPES_SHAPE_HPP

#include "fields/field.hpp"
#include "fields/handles.hpp"
#include "fields/numbers.hpp"
#include "fields/points.hpp"
#include "fields/subclass.hpp"
#include "fields/text.hpp"
#include "record.hpp"
#include "shapes.hpp"

#include "dxfio/entities/shape.hpp"
#include "dxfio/vector.hpp"

#include <string_view>
#include <tuple>

namespace DxfIO
{
  template <>
  inline constexpr auto Shapes<Entities::Shape> = std::tuple{
      Entities::Part<Entities::Shape>,
      Fields::Subclass{.Marker = "AcDbShape"},
      Fields::Field{
          .Code     = 39,
          .Member   = &Entities::Shape::Thickness,
          .Fallback = 0.0,
      },
      Fields::Field{.Code = 10, .Member = &Entities::Shape::Position},
      Fields::Field{.Code = 40, .Member = &Entities::Shape::Size},
      Fields::Field{.Code = 2, .Member = &Entities::Shape::Name},
      Fields::Field{
          .Code     = 50,
          .Member   = &Entities::Shape::Rotation,
          .Fallback = 0.0,
      },
      Fields::Field{
          .Code     = 41,
          .Member   = &Entities::Shape::Width,
          .Fallback = 1.0,
      },
      Fields::Field{
          .Code     = 51,
          .Member   = &Entities::Shape::Oblique,
          .Fallback = 0.0,
      },
      Fields::Field{
          .Code     = 210,
          .Member   = &Entities::Shape::Extrusion,
          .Fallback = Up,
      },
  };

  template <>
  inline constexpr std::string_view Type<Entities::Shape> = "SHAPE";
} // namespace DxfIO

#endif /* DXFIO_ENTITIES_SHAPES_SHAPE_HPP */
