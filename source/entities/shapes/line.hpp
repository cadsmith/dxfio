#ifndef DXFIO_ENTITIES_SHAPES_LINE_HPP
#define DXFIO_ENTITIES_SHAPES_LINE_HPP

#include "fields/field.hpp"
#include "fields/numbers.hpp"
#include "fields/points.hpp"
#include "fields/subclass.hpp"
#include "record.hpp"
#include "shapes.hpp"

#include "dxfio/entities/line.hpp"
#include "dxfio/vector.hpp"

#include <string_view>
#include <tuple>

namespace DxfIO
{
  template <>
  inline constexpr auto Shapes<Entities::Line> = std::tuple{
      Entities::Part<Entities::Line>,
      Fields::Subclass{.Marker = "AcDbLine"},
      Fields::Field{
          .Code     = 39,
          .Member   = &Entities::Line::Thickness,
          .Fallback = 0.0,
      },
      Fields::Field{.Code = 10, .Member = &Entities::Line::Start},
      Fields::Field{.Code = 11, .Member = &Entities::Line::End},
      Fields::Field{
          .Code     = 210,
          .Member   = &Entities::Line::Extrusion,
          .Fallback = Up,
      },
  };

  template <>
  inline constexpr std::string_view Type<Entities::Line> = "LINE";
} // namespace DxfIO

#endif /* DXFIO_ENTITIES_SHAPES_LINE_HPP */
