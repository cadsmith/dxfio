#ifndef DXFIO_ENTITIES_SHAPES_HATCH_PATH_HPP
#define DXFIO_ENTITIES_SHAPES_HATCH_PATH_HPP

#include "edge.hpp"
#include "fields/count.hpp"
#include "fields/custom.hpp"
#include "fields/each.hpp"
#include "fields/field.hpp"
#include "fields/handles.hpp"
#include "fields/many.hpp"
#include "fields/numbers.hpp"
#include "fields/tally.hpp"
#include "fields/when.hpp"
#include "masked.hpp"
#include "shapes.hpp"

#include "dxfio/entities/hatch/path.hpp"
#include "dxfio/failure.hpp"

#include <expected>
#include <tuple>

namespace DxfIO
{
namespace Entities
{
inline constexpr int Polygonal = 2;

inline constexpr int Pathed = 92;

inline auto Stated(const Path &one) -> int
{
  const int cleared = one.Flags - Masked(one.Flags, Polygonal);
  if (!one.Vertices.empty())
  {
    return cleared + Polygonal;
  }
  if (!one.Edges.empty())
  {
    return cleared;
  }
  return one.Flags;
}

inline void Stating(Path &one, Fields::Walk &in)
{
  one.Flags = Fields::Value<int>::Read(in, Pathed).value_or(0);
}

inline auto Stamping(const Path &one, Fields::Writing &to) -> std::expected<void, Failure>
{
  return Fields::Value<int>::Write(to, Pathed, Stated(one));
}

inline auto Straight(const Path &one) -> bool
{
  return Masked(Stated(one), Polygonal) != 0;
}

inline auto Edged(const Path &one) -> bool
{
  return !Straight(one);
}

inline constexpr std::tuple Polygon{
    Fields::Field{.Code = 72, .Member = &Path::Bulged},
    Fields::Field{.Code = 73, .Member = &Path::Closed},
    Fields::Count{.Code = 93, .Member = &Path::Vertices},
    Fields::Each{
        .Opener = 10,
        .Member = &Path::Vertices,
        .Fields = Shapes<Edges::Node>,
    },
};

inline constexpr std::tuple Outline{
    Fields::Count{.Code = 93, .Member = &Path::Edges},
    Fields::Each{
        .Opener = 72,
        .Member = &Path::Edges,
        .Fields = Shapes<Edges::Edge>,
    },
};
} // namespace Entities

template <>
inline constexpr auto Shapes<Entities::Path> = std::tuple{
    Fields::Custom<Entities::Path>{
        .Read = Entities::Stating,
        .Write = Entities::Stamping,
    },
    Fields::When{.Test = Entities::Straight, .Fields = Entities::Polygon},
    Fields::When{.Test = Entities::Edged, .Fields = Entities::Outline},
    Fields::Tally{.Code = 97, .Member = &Entities::Path::Sources},
    Fields::Many{.Code = 330, .Member = &Entities::Path::Sources},
};
} // namespace DxfIO

#endif /* DXFIO_ENTITIES_SHAPES_HATCH_PATH_HPP */
