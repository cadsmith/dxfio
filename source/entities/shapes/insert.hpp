#ifndef DXFIO_ENTITIES_SHAPES_INSERT_HPP
#define DXFIO_ENTITIES_SHAPES_INSERT_HPP

#include "attrib.hpp"
#include "fields/either.hpp"
#include "fields/field.hpp"
#include "fields/numbers.hpp"
#include "fields/optional.hpp"
#include "fields/part.hpp"
#include "fields/points.hpp"
#include "fields/text.hpp"
#include "record.hpp"
#include "sequence.hpp"
#include "shapes.hpp"

#include "dxfio/entities/insert.hpp"
#include "dxfio/vector.hpp"

#include <array>
#include <cstddef>
#include <string_view>
#include <tuple>

namespace DxfIO
{
  namespace Entities
  {
    inline auto Marked(const Insert &one) -> std::size_t
    {
      return one.Columns.value_or(1) > 1 || one.Rows.value_or(1) > 1 ? 1 : 0;
    }

    inline constexpr auto Scales = std::tuple{
        Fields::Field{.Code = 41, .Member = &Vector::X, .Fallback = 1.0},
        Fields::Field{.Code = 42, .Member = &Vector::Y, .Fallback = 1.0},
        Fields::Field{.Code = 43, .Member = &Vector::Z, .Fallback = 1.0},
    };
  } // namespace Entities

  template <>
  inline constexpr auto Shapes<Entities::Insert> = std::tuple{
      Entities::Part<Entities::Insert>,
      Fields::Either<Entities::Insert, 2>{
          .Markers = {"AcDbBlockReference", "AcDbMInsertBlock"},
          .Pick    = Entities::Marked,
      },
      Fields::Field{
          .Code     = 66,
          .Member   = &Entities::Insert::Follows,
          .Fallback = false,
      },
      Fields::Field{.Code = 2, .Member = &Entities::Insert::Block},
      Fields::Field{.Code = 10, .Member = &Entities::Insert::Position},
      Fields::Part{
          .Member = &Entities::Insert::Scale,
          .Fields = Entities::Scales,
      },
      Fields::Field{
          .Code     = 50,
          .Member   = &Entities::Insert::Rotation,
          .Fallback = 0.0,
      },
      Fields::Field{.Code = 70, .Member = &Entities::Insert::Columns},
      Fields::Field{.Code = 71, .Member = &Entities::Insert::Rows},
      Fields::Field{
          .Code     = 44,
          .Member   = &Entities::Insert::Across,
          .Fallback = 0.0,
      },
      Fields::Field{
          .Code     = 45,
          .Member   = &Entities::Insert::Down,
          .Fallback = 0.0,
      },
      Fields::Field{
          .Code     = 210,
          .Member   = &Entities::Insert::Extrusion,
          .Fallback = Up,
      },
  };

  template <>
  inline constexpr std::string_view Type<Entities::Insert> = "INSERT";

  template <>
  inline void Follow(Entities::Insert &one, std::span<const File::Record> &rest,
                     const Fields::Reading &with)
  {
    if (!one.Follows)
    {
      return;
    }
    Entities::Gather(one.Attributes, rest, with);
    if (!one.Attributes.Closing)
    {
      with.Tell(Issue{.Record = one.Record.Base.Self, .What = "INSERT has no SEQEND"});
    }
  }

  template <>
  inline auto Follow(Fields::Writing &to, const Entities::Insert &one)
      -> std::expected<void, Failure>
  {
    return Entities::Give(to, one.Attributes);
  }
} // namespace DxfIO

#endif /* DXFIO_ENTITIES_SHAPES_INSERT_HPP */
