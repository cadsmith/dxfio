#ifndef DXFIO_ENTITIES_SHAPES_MLINE_HPP
#define DXFIO_ENTITIES_SHAPES_MLINE_HPP

#include "fields/count.hpp"
#include "fields/each.hpp"
#include "fields/every.hpp"
#include "fields/field.hpp"
#include "fields/handles.hpp"
#include "fields/many.hpp"
#include "fields/numbers.hpp"
#include "fields/part.hpp"
#include "fields/points.hpp"
#include "fields/subclass.hpp"
#include "fields/tally.hpp"
#include "fields/text.hpp"
#include "record.hpp"
#include "shapes.hpp"

#include "dxfio/entities/mline.hpp"
#include "dxfio/vector.hpp"

#include <string_view>
#include <tuple>

namespace DxfIO
{
  template <>
  inline constexpr auto Shapes<Entities::Strand> = std::tuple{
      Fields::Tally{.Code = 74, .Member = &Entities::Strand::Spans},
      Fields::Many{.Code = 41, .Member = &Entities::Strand::Spans},
      Fields::Tally{.Code = 75, .Member = &Entities::Strand::Fills},
      Fields::Many{.Code = 42, .Member = &Entities::Strand::Fills},
  };

  template <>
  inline constexpr auto Shapes<Entities::Joint> = std::tuple{
      Fields::Field{.Code = 11, .Member = &Entities::Joint::Position},
      Fields::Field{.Code = 12, .Member = &Entities::Joint::Direction},
      Fields::Field{.Code = 13, .Member = &Entities::Joint::Miter},
      Fields::Every{
          .Opener = 74,
          .Member = &Entities::Joint::Strands,
          .Fields = Shapes<Entities::Strand>,
      },
  };

  template <>
  inline constexpr auto Shapes<Entities::Web> = std::tuple{
      Fields::Subclass{.Marker = "AcDbMline"},
      Fields::Field{.Code = 2, .Member = &Entities::Web::Style},
      Fields::Field{.Code = 340, .Member = &Entities::Web::Styled},
      Fields::Field{.Code = 40, .Member = &Entities::Web::Scale},
      Fields::Field{.Code = 70, .Member = &Entities::Web::Justification},
      Fields::Field{.Code = 71, .Member = &Entities::Web::Flags},
      Fields::Count{.Code = 72, .Member = &Entities::Web::Joints},
      Fields::Field{.Code = 73, .Member = &Entities::Web::Lines},
      Fields::Field{.Code = 10, .Member = &Entities::Web::Start},
      Fields::Field{.Code = 210, .Member = &Entities::Web::Normal},
      Fields::Each{
          .Opener = 11,
          .Member = &Entities::Web::Joints,
          .Fields = Shapes<Entities::Joint>,
      },
  };

  template <>
  inline constexpr auto Shapes<Entities::MLine> = std::tuple{
      Entities::Part<Entities::MLine>,
      Fields::Part{
          .Member = &Entities::MLine::Web,
          .Fields = Shapes<Entities::Web>,
      },
  };

  template <>
  inline constexpr std::string_view Type<Entities::MLine> = "MLINE";
} // namespace DxfIO

#endif /* DXFIO_ENTITIES_SHAPES_MLINE_HPP */
