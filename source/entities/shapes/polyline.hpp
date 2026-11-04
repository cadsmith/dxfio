#ifndef DXFIO_ENTITIES_SHAPES_POLYLINE_HPP
#define DXFIO_ENTITIES_SHAPES_POLYLINE_HPP

#include "fields/either.hpp"
#include "fields/field.hpp"
#include "fields/numbers.hpp"
#include "fields/points.hpp"
#include "masked.hpp"
#include "record.hpp"
#include "sequence.hpp"
#include "shapes.hpp"
#include "vertex.hpp"

#include "dxfio/entities/polyline.hpp"
#include "dxfio/vector.hpp"

#include <array>
#include <cstddef>
#include <format>
#include <string_view>
#include <tuple>

namespace DxfIO
{
namespace Entities
{
inline auto Marked(const Polyline &one) -> std::size_t
{
  constexpr int Spatial = 8;
  constexpr int Meshed = 16;
  constexpr int Faced = 64;
  if (Masked(one.Flags, Faced) != 0)
  {
    return 3;
  }
  if (Masked(one.Flags, Meshed) != 0)
  {
    return 2;
  }
  return Masked(one.Flags, Spatial) != 0 ? 1 : 0;
}
} // namespace Entities

template <>
inline constexpr auto Shapes<Entities::Polyline> = std::tuple{
    Entities::Part<Entities::Polyline>,
    Fields::Either<Entities::Polyline, 4>{
        .Markers =
            {
                "AcDb2dPolyline",
                "AcDb3dPolyline",
                "AcDbPolygonMesh",
                "AcDbPolyFaceMesh",
            },
        .Pick = Entities::Marked,
    },
    Fields::Field{.Code = 66, .Member = &Entities::Polyline::Follows},
    Fields::Field{.Code = 10, .Member = &Entities::Polyline::Position},
    Fields::Field{
        .Code = 39,
        .Member = &Entities::Polyline::Thickness,
        .Fallback = 0.0,
    },
    Fields::Field{
        .Code = 70,
        .Member = &Entities::Polyline::Flags,
        .Fallback = 0,
    },
    Fields::Field{
        .Code = 40,
        .Member = &Entities::Polyline::Start,
        .Fallback = 0.0,
    },
    Fields::Field{
        .Code = 41,
        .Member = &Entities::Polyline::End,
        .Fallback = 0.0,
    },
    Fields::Field{
        .Code = 71,
        .Member = &Entities::Polyline::Rows,
        .Fallback = 0,
    },
    Fields::Field{
        .Code = 72,
        .Member = &Entities::Polyline::Columns,
        .Fallback = 0,
    },
    Fields::Field{
        .Code = 73,
        .Member = &Entities::Polyline::Density,
        .Fallback = 0,
    },
    Fields::Field{
        .Code = 74,
        .Member = &Entities::Polyline::Depth,
        .Fallback = 0,
    },
    Fields::Field{
        .Code = 75,
        .Member = &Entities::Polyline::Curve,
        .Fallback = 0,
    },
    Fields::Field{
        .Code = 210,
        .Member = &Entities::Polyline::Extrusion,
        .Fallback = Up,
    },
};

template <>
inline constexpr std::string_view Type<Entities::Polyline> = "POLYLINE";

template <>
inline void Follow(Entities::Polyline &one, std::span<const File::Record> &rest, const Fields::Reading &with)
{
  Entities::Gather(one.Vertices, rest, with);
  if (!one.Vertices.Closing)
  {
    with.Tell(Issue{
        .Record = one.Record.Base.Self,
        .What = "POLYLINE has no SEQEND",
    });
  }
}

template <>
inline auto Follow(Fields::Writing &to, const Entities::Polyline &one) -> std::expected<void, Failure>
{
  return Entities::Give(to, one.Vertices);
}
} // namespace DxfIO

#endif /* DXFIO_ENTITIES_SHAPES_POLYLINE_HPP */
