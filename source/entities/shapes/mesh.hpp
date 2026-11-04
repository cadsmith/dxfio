#ifndef DXFIO_ENTITIES_SHAPES_MESH_HPP
#define DXFIO_ENTITIES_SHAPES_MESH_HPP

#include "fields/custom.hpp"
#include "fields/field.hpp"
#include "fields/many.hpp"
#include "fields/numbers.hpp"
#include "fields/owned.hpp"
#include "fields/part.hpp"
#include "fields/picture.hpp"
#include "fields/points.hpp"
#include "fields/sequence.hpp"
#include "fields/tally.hpp"
#include "record.hpp"
#include "shapes.hpp"

#include "dxfio/entities/mesh.hpp"
#include "dxfio/failure.hpp"

#include <cstdint>
#include <expected>
#include <string_view>
#include <tuple>

namespace DxfIO
{
namespace Entities
{
inline constexpr int Paired = 94;

inline constexpr int Tailed = 90;

inline constexpr Fields::Many<Net, int> Rungs{.Code = Tailed, .Member = &Net::Edges};

inline auto Odd(const Net &one) -> bool
{
  return one.Edges.size() % 2 != 0;
}

inline void Pairs(Net &one, Fields::Walk &in)
{
  static_cast<void>(Fields::Value<std::int64_t>::Read(in, Paired));
  Fields::Take(Rungs, one, in);
  if (Odd(one))
  {
    in.Refuse();
  }
}

inline auto Pairing(const Net &one, Fields::Writing &to) -> std::expected<void, Failure>
{
  if (Odd(one))
  {
    return std::unexpected{Failure{.What = "a MESH's edges hold an odd count of vertex indices"}};
  }
  const auto size = static_cast<std::int64_t>(one.Edges.size() / 2);
  return Fields::Value<std::int64_t>::Write(to, Paired, size)
      .and_then([&] -> auto { return Fields::Put(Rungs, one, to); });
}

inline void Overriding(Net &one, Fields::Walk &in)
{
  if (!in.Holds(Tailed))
  {
    return;
  }
  one.Overridden = Fields::Value<int>::Read(in, Tailed).value_or(0);
  one.Overrides = in.Rest();
  in.Spend();
}

inline auto Overrode(const Net &one, Fields::Writing &to) -> std::expected<void, Failure>
{
  return Fields::Value<int>::Write(to, Tailed, one.Overridden)
      .and_then([&] -> auto { return Fields::Put(to, one.Overrides); });
}
} // namespace Entities

template <>
inline constexpr auto Shapes<Entities::Net> = std::tuple{
    Fields::Sequence{.Marker = "AcDbSubDMesh"},
    Fields::Field{.Code = 71, .Member = &Entities::Net::Version},
    Fields::Field{.Code = 72, .Member = &Entities::Net::Blend},
    Fields::Field{.Code = 91, .Member = &Entities::Net::Level},
    Fields::Tally{.Code = 92, .Member = &Entities::Net::Vertices},
    Fields::Many{.Code = 10, .Member = &Entities::Net::Vertices},
    Fields::Tally{.Code = 93, .Member = &Entities::Net::Faces},
    Fields::Many{.Code = 90, .Member = &Entities::Net::Faces},
    Fields::Custom<Entities::Net>{
        .Read = Entities::Pairs,
        .Write = Entities::Pairing,
    },
    Fields::Tally{.Code = 95, .Member = &Entities::Net::Creases},
    Fields::Many{.Code = 140, .Member = &Entities::Net::Creases},
    Fields::Custom<Entities::Net>{
        .Read = Entities::Overriding,
        .Write = Entities::Overrode,
    },
};

template <>
inline constexpr auto Shapes<Entities::Mesh> = std::tuple{
    Entities::Part<Entities::Mesh>,
    Fields::Picture{.Member = &Entities::Mesh::Picture},
    Fields::Part{
        .Member = &Entities::Mesh::Net,
        .Fields = Shapes<Entities::Net>,
    },
};

template <>
inline constexpr std::string_view Type<Entities::Mesh> = "MESH";
} // namespace DxfIO

#endif /* DXFIO_ENTITIES_SHAPES_MESH_HPP */
