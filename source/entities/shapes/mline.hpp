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
  inline constexpr auto Shapes<Entities::MLineData> = std::tuple{
      Fields::Subclass{.Marker = "AcDbMline"},
      Fields::Field{.Code = 2, .Member = &Entities::MLineData::Style},
      Fields::Field{.Code = 340, .Member = &Entities::MLineData::Styled},
      Fields::Field{.Code = 40, .Member = &Entities::MLineData::Scale},
      Fields::Field{.Code = 70, .Member = &Entities::MLineData::Justification},
      Fields::Field{.Code = 71, .Member = &Entities::MLineData::Flags},
      Fields::Count{.Code = 72, .Member = &Entities::MLineData::Joints},
      Fields::Field{.Code = 73, .Member = &Entities::MLineData::Lines},
      Fields::Field{.Code = 10, .Member = &Entities::MLineData::Start},
      Fields::Field{.Code = 210, .Member = &Entities::MLineData::Normal},
      Fields::Each{
          .Opener = 11,
          .Member = &Entities::MLineData::Joints,
          .Fields = Shapes<Entities::Joint>,
      },
  };

  template <>
  inline constexpr auto Shapes<Entities::MLine> = std::tuple{
      Entities::Part<Entities::MLine>,
      Fields::Part{
          .Member = &Entities::MLine::Data,
          .Fields = Shapes<Entities::MLineData>,
      },
  };

  template <>
  inline constexpr std::string_view Type<Entities::MLine> = "MLINE";
} // namespace DxfIO

#endif /* DXFIO_ENTITIES_SHAPES_MLINE_HPP */
