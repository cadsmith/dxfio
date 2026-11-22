#ifndef DXFIO_ENTITIES_SHAPES_LEADER_HPP
#define DXFIO_ENTITIES_SHAPES_LEADER_HPP

#include "fields/field.hpp"
#include "fields/handles.hpp"
#include "fields/many.hpp"
#include "fields/numbers.hpp"
#include "fields/optional.hpp"
#include "fields/part.hpp"
#include "fields/points.hpp"
#include "fields/subclass.hpp"
#include "fields/tally.hpp"
#include "fields/text.hpp"
#include "record.hpp"
#include "shapes.hpp"

#include "dxfio/entities/leader.hpp"
#include "dxfio/handle.hpp"
#include "dxfio/vector.hpp"

#include <string_view>
#include <tuple>

namespace DxfIO
{
  template <>
  inline constexpr auto Shapes<Entities::LeaderData> = std::tuple{
      Fields::Subclass{.Marker = "AcDbLeader"},
      Fields::Field{.Code = 3, .Member = &Entities::LeaderData::Style},
      Fields::Field{.Code = 71, .Member = &Entities::LeaderData::Arrow, .Fallback = 1},
      Fields::Field{.Code = 72, .Member = &Entities::LeaderData::Path, .Fallback = 0},
      Fields::Field{
          .Code     = 73,
          .Member   = &Entities::LeaderData::Annotation,
          .Fallback = 3,
      },
      Fields::Field{
          .Code     = 74,
          .Member   = &Entities::LeaderData::Direction,
          .Fallback = 1,
      },
      Fields::Field{.Code = 75, .Member = &Entities::LeaderData::Hook, .Fallback = 0},
      Fields::Field{.Code = 40, .Member = &Entities::LeaderData::Height},
      Fields::Field{.Code = 41, .Member = &Entities::LeaderData::Width},
      Fields::Tally{.Code = 76, .Member = &Entities::LeaderData::Points},
      Fields::Many{.Code = 10, .Member = &Entities::LeaderData::Points},
      Fields::Field{.Code = 77, .Member = &Entities::LeaderData::Color},
      Fields::Field{.Code = 340, .Member = &Entities::LeaderData::Note, .Fallback = Handle{}},
      Fields::Field{
          .Code     = 210,
          .Member   = &Entities::LeaderData::Normal,
          .Fallback = Up,
      },
      Fields::Field{
          .Code     = 211,
          .Member   = &Entities::LeaderData::Horizontal,
          .Fallback = Vector{.X = 1},
      },
      Fields::Field{.Code = 212, .Member = &Entities::LeaderData::Block, .Fallback = Vector{}},
      Fields::Field{.Code = 213, .Member = &Entities::LeaderData::Offset, .Fallback = Vector{}},
  };

  template <>
  inline constexpr auto Shapes<Entities::Leader> = std::tuple{
      Entities::Part<Entities::Leader>,
      Fields::Part{
          .Member = &Entities::Leader::Data,
          .Fields = Shapes<Entities::LeaderData>,
      },
  };

  template <>
  inline constexpr std::string_view Type<Entities::Leader> = "LEADER";
} // namespace DxfIO

#endif /* DXFIO_ENTITIES_SHAPES_LEADER_HPP */
