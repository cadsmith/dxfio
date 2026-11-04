#ifndef DXFIO_ENTITIES_SHAPES_VERTEX_HPP
#define DXFIO_ENTITIES_SHAPES_VERTEX_HPP

#include "fields/either.hpp"
#include "fields/field.hpp"
#include "fields/numbers.hpp"
#include "fields/optional.hpp"
#include "fields/points.hpp"
#include "fields/subclass.hpp"
#include "fields/when.hpp"
#include "identifier.hpp"
#include "masked.hpp"
#include "record.hpp"
#include "shapes.hpp"

#include "dxfio/entities/vertex.hpp"

#include <array>
#include <cstddef>
#include <string_view>
#include <tuple>

namespace DxfIO
{
namespace Entities
{
inline auto Placed(const Vertex &one) -> bool
{
  constexpr int Faced = 128;
  constexpr int Meshed = 64;
  return Masked(one.Flags, Faced + Meshed) != Faced;
}

inline auto Marked(const Vertex &one) -> std::size_t
{
  constexpr int Spatial = 32;
  constexpr int Meshed = 64;
  constexpr int Faced = 128;
  if (!Placed(one))
  {
    return 4;
  }
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
inline constexpr auto Shapes<Entities::Vertex> = std::tuple{
    Entities::Part<Entities::Vertex>,
    Fields::When{
        .Test = Entities::Placed,
        .Fields = std::tuple{Fields::Subclass{.Marker = "AcDbVertex"}},
    },
    Fields::Either<Entities::Vertex, 5>{
        .Markers =
            {
                "AcDb2dVertex",
                "AcDb3dPolylineVertex",
                "AcDbPolygonMeshVertex",
                "AcDbPolyFaceMeshVertex",
                "AcDbFaceRecord",
            },
        .Pick = Entities::Marked,
    },
    Fields::Field{.Code = 10, .Member = &Entities::Vertex::Position},
    Fields::Field{.Code = 40, .Member = &Entities::Vertex::Start},
    Fields::Field{.Code = 41, .Member = &Entities::Vertex::End},
    Fields::Field{
        .Code = 42,
        .Member = &Entities::Vertex::Bulge,
        .Fallback = 0.0,
    },
    Entities::Identifier<Entities::Vertex>,
    Fields::Field{
        .Code = 70,
        .Member = &Entities::Vertex::Flags,
        .Fallback = 0,
    },
    Fields::Field{
        .Code = 50,
        .Member = &Entities::Vertex::Tangent,
        .Fallback = 0.0,
    },
    Fields::Field{
        .Code = 71,
        .Member = &Entities::Vertex::First,
        .Fallback = 0,
    },
    Fields::Field{
        .Code = 72,
        .Member = &Entities::Vertex::Second,
        .Fallback = 0,
    },
    Fields::Field{
        .Code = 73,
        .Member = &Entities::Vertex::Third,
        .Fallback = 0,
    },
    Fields::Field{
        .Code = 74,
        .Member = &Entities::Vertex::Fourth,
        .Fallback = 0,
    },
};

template <>
inline constexpr std::string_view Type<Entities::Vertex> = "VERTEX";
} // namespace DxfIO

#endif /* DXFIO_ENTITIES_SHAPES_VERTEX_HPP */
