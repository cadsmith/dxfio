#ifndef DXFIO_ENTITIES_SHAPES_FILLED_HPP
#define DXFIO_ENTITIES_SHAPES_FILLED_HPP

#include "fields/corners.hpp"
#include "fields/field.hpp"
#include "fields/numbers.hpp"
#include "fields/points.hpp"
#include "fields/subclass.hpp"
#include "record.hpp"
#include "shapes.hpp"

#include "dxfio/corners.hpp"
#include "dxfio/vector.hpp"

#include <tuple>

namespace DxfIO::Entities
{
  template <class KindT>
  inline constexpr auto Filled = std::tuple{
      Part<KindT>,
      Fields::Subclass{.Marker = "AcDbTrace"},
      Fields::Field<KindT, Corners>{.Code = 10, .Member = &KindT::Corners},
      Fields::Field<KindT, double, double>{
          .Code     = 39,
          .Member   = &KindT::Thickness,
          .Fallback = 0.0,
      },
      Fields::Field<KindT, Vector, Vector>{
          .Code     = 210,
          .Member   = &KindT::Extrusion,
          .Fallback = Up,
      },
  };
} // namespace DxfIO::Entities

#endif /* DXFIO_ENTITIES_SHAPES_FILLED_HPP */
