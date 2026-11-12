#ifndef DXFIO_TABLES_SHAPES_AXES_HPP
#define DXFIO_TABLES_SHAPES_AXES_HPP

#include "fields/field.hpp"
#include "fields/handles.hpp"
#include "fields/numbers.hpp"
#include "fields/points.hpp"
#include "shapes.hpp"

#include "dxfio/tables/axes.hpp"

#include <tuple>

namespace DxfIO
{
  template <>
  inline constexpr auto Shapes<Tables::Axes> = std::tuple{
      Fields::Field{.Code = 110, .Member = &Tables::Axes::Origin},
      Fields::Field{.Code = 111, .Member = &Tables::Axes::X},
      Fields::Field{.Code = 112, .Member = &Tables::Axes::Y},
      Fields::Field{.Code = 345, .Member = &Tables::Axes::Named, .Fallback = Handle{}},
      Fields::Field{.Code = 346, .Member = &Tables::Axes::Basis, .Fallback = Handle{}},
      Fields::Field{.Code = 79, .Member = &Tables::Axes::Ortho},
      Fields::Field{.Code = 146, .Member = &Tables::Axes::Elevation},
  };
} // namespace DxfIO

#endif /* DXFIO_TABLES_SHAPES_AXES_HPP */
