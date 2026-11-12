#ifndef DXFIO_ENTITIES_SHAPES_DIMENSION_MEASURE_HPP
#define DXFIO_ENTITIES_SHAPES_DIMENSION_MEASURE_HPP

#include "fields/field.hpp"
#include "fields/first.hpp"
#include "fields/numbers.hpp"
#include "fields/optional.hpp"
#include "fields/points.hpp"
#include "fields/since.hpp"
#include "fields/skip.hpp"
#include "fields/subclass.hpp"
#include "fields/text.hpp"
#include "shapes.hpp"

#include "dxfio/entities/dimension/measure.hpp"
#include "dxfio/vector.hpp"
#include "dxfio/version.hpp"

#include <string_view>
#include <tuple>

namespace DxfIO
{
  namespace Entities
  {
    inline constexpr std::tuple Settings{
        Fields::Field{.Code = 71, .Member = &Measure::Attachment},
        Fields::Field{.Code = 72, .Member = &Measure::Spacing},
        Fields::Field{.Code = 41, .Member = &Measure::Factor},
        Fields::Field{.Code = 42, .Member = &Measure::Measured},
    };

    inline constexpr std::tuple Bits{
        Fields::Field{.Code = 73, .Member = &Measure::Above},
        Fields::Field{.Code = 74, .Member = &Measure::First},
        Fields::Field{.Code = 75, .Member = &Measure::Second},
    };
  } // namespace Entities

  template <>
  inline constexpr auto Shapes<Entities::Measure> = std::tuple{
      Fields::Subclass{.Marker = "AcDbDimension"},
      Fields::Field{
          .Code   = 280,
          .Member = &Entities::Measure::Version,
          .Since  = Version::R2010,
      },
      Fields::Field{
          .Code     = 2,
          .Member   = &Entities::Measure::Block,
          .Fallback = std::string_view{},
      },
      Fields::Field{.Code = 10, .Member = &Entities::Measure::Point},
      Fields::Field{.Code = 11, .Member = &Entities::Measure::Middle},
      Fields::Field{.Code = 12, .Member = &Entities::Measure::Clone},
      Fields::First{
          .Code   = 70,
          .Fields = std::tuple{Fields::Field{
              .Code   = 70,
              .Member = &Entities::Measure::Flags,
          }},
      },
      Fields::Skip{.Code = 70},
      Fields::Field{
          .Code     = 1,
          .Member   = &Entities::Measure::Text,
          .Fallback = std::string_view{},
      },
      Fields::Since{.Release = Version::R2000, .Fields = Entities::Settings},
      Fields::Since{.Release = Version::R2007, .Fields = Entities::Bits},
      Fields::Field{.Code = 52, .Member = &Entities::Measure::Oblique},
      Fields::Field{.Code = 53, .Member = &Entities::Measure::Rotation},
      Fields::Field{.Code = 54, .Member = &Entities::Measure::Turned},
      Fields::Field{.Code = 51, .Member = &Entities::Measure::Horizontal},
      Fields::Field{
          .Code     = 210,
          .Member   = &Entities::Measure::Normal,
          .Fallback = Up,
      },
      Fields::Field{
          .Code     = 3,
          .Member   = &Entities::Measure::Style,
          .Fallback = std::string_view{},
      },
  };
} // namespace DxfIO

#endif /* DXFIO_ENTITIES_SHAPES_DIMENSION_MEASURE_HPP */
