#ifndef DXFIO_ENTITIES_SHAPES_ROUND_HPP
#define DXFIO_ENTITIES_SHAPES_ROUND_HPP

#include "fields/field.hpp"
#include "fields/numbers.hpp"
#include "fields/points.hpp"
#include "fields/subclass.hpp"
#include "record.hpp"
#include "shapes.hpp"

#include "dxfio/entities/record.hpp"
#include "dxfio/vector.hpp"

#include <tuple>

namespace DxfIO::Entities
{
  template <class KindT>
  inline constexpr auto Round = std::tuple{
      Part<KindT>,
      Fields::Subclass{.Marker = "AcDbCircle"},
      Fields::Field<KindT, double, double>{
          .Code     = 39,
          .Member   = &KindT::Thickness,
          .Fallback = 0.0,
      },
      Fields::Field<KindT, Vector>{.Code = 10, .Member = &KindT::Center},
      Fields::Field<KindT, double>{.Code = 40, .Member = &KindT::Radius},
      Fields::Field<KindT, Vector, Vector>{
          .Code     = 210,
          .Member   = &KindT::Extrusion,
          .Fallback = Up,
      },
  };
} // namespace DxfIO::Entities

#endif /* DXFIO_ENTITIES_SHAPES_ROUND_HPP */
