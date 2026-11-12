#ifndef DXFIO_ENTITIES_SHAPES_TOLERANCE_HPP
#define DXFIO_ENTITIES_SHAPES_TOLERANCE_HPP

#include "fields/field.hpp"
#include "fields/numbers.hpp"
#include "fields/points.hpp"
#include "fields/subclass.hpp"
#include "fields/text.hpp"
#include "record.hpp"
#include "shapes.hpp"

#include "dxfio/entities/tolerance.hpp"
#include "dxfio/vector.hpp"

#include <string_view>
#include <tuple>

namespace DxfIO
{
  template <>
  inline constexpr auto Shapes<Entities::Tolerance> = std::tuple{
      Entities::Part<Entities::Tolerance>,
      Fields::Subclass{.Marker = "AcDbFcf"},
      Fields::Field{.Code = 3, .Member = &Entities::Tolerance::Style},
      Fields::Field{.Code = 10, .Member = &Entities::Tolerance::Position},
      Fields::Field{.Code = 1, .Member = &Entities::Tolerance::Text},
      Fields::Field{
          .Code     = 11,
          .Member   = &Entities::Tolerance::Direction,
          .Fallback = Vector{.X = 1},
      },
      Fields::Field{
          .Code     = 210,
          .Member   = &Entities::Tolerance::Normal,
          .Fallback = Up,
      },
  };

  template <>
  inline constexpr std::string_view Type<Entities::Tolerance> = "TOLERANCE";
} // namespace DxfIO

#endif /* DXFIO_ENTITIES_SHAPES_TOLERANCE_HPP */
