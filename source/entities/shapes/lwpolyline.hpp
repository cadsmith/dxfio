#ifndef DXFIO_ENTITIES_SHAPES_LWPOLYLINE_HPP
#define DXFIO_ENTITIES_SHAPES_LWPOLYLINE_HPP

#include "fields/count.hpp"
#include "fields/every.hpp"
#include "fields/field.hpp"
#include "fields/flat.hpp"
#include "fields/numbers.hpp"
#include "fields/optional.hpp"
#include "fields/points.hpp"
#include "fields/subclass.hpp"
#include "fields/widths.hpp"
#include "identifier.hpp"
#include "record.hpp"
#include "shapes.hpp"

#include "dxfio/entities/lwpolyline.hpp"
#include "dxfio/vector.hpp"

#include <optional>
#include <string_view>
#include <tuple>

namespace DxfIO
{
  template <>
  inline constexpr auto Shapes<Entities::LwPolyline::Vertex> = std::tuple{
      Fields::Flat{.Code = 10, .Member = &Entities::LwPolyline::Vertex::Position},
      Fields::Field{.Code = 40, .Member = &Entities::LwPolyline::Vertex::Widths},
      Fields::Field{
          .Code     = 42,
          .Member   = &Entities::LwPolyline::Vertex::Bulge,
          .Fallback = 0.0,
      },
      Entities::Identifier<Entities::LwPolyline::Vertex>,
  };

  template <>
  inline constexpr auto Shapes<Entities::LwPolyline> = std::tuple{
      Entities::Part<Entities::LwPolyline>,
      Fields::Subclass{.Marker = "AcDbPolyline"},
      Fields::Count{.Code = 90, .Member = &Entities::LwPolyline::Vertices},
      Fields::Field{.Code = 70, .Member = &Entities::LwPolyline::Flags},
      Fields::Field{
          .Code     = 43,
          .Member   = &Entities::LwPolyline::Width,
          .Fallback = Fields::Empty{},
      },
      Fields::Field{
          .Code     = 38,
          .Member   = &Entities::LwPolyline::Elevation,
          .Fallback = 0.0,
      },
      Fields::Field{
          .Code     = 39,
          .Member   = &Entities::LwPolyline::Thickness,
          .Fallback = 0.0,
      },
      Fields::Every{
          .Opener = 10,
          .Member = &Entities::LwPolyline::Vertices,
          .Fields = Shapes<Entities::LwPolyline::Vertex>,
      },
      Fields::Field{
          .Code     = 210,
          .Member   = &Entities::LwPolyline::Extrusion,
          .Fallback = Up,
      },
  };

  template <>
  inline constexpr std::string_view Type<Entities::LwPolyline> = "LWPOLYLINE";
} // namespace DxfIO

#endif /* DXFIO_ENTITIES_SHAPES_LWPOLYLINE_HPP */
