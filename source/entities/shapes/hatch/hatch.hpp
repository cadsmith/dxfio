#ifndef DXFIO_ENTITIES_SHAPES_HATCH_HATCH_HPP
#define DXFIO_ENTITIES_SHAPES_HATCH_HATCH_HPP

#include "../record.hpp"
#include "edge.hpp"
#include "fields/count.hpp"
#include "fields/each.hpp"
#include "fields/field.hpp"
#include "fields/many.hpp"
#include "fields/maybe.hpp"
#include "fields/numbers.hpp"
#include "fields/optional.hpp"
#include "fields/part.hpp"
#include "fields/points.hpp"
#include "fields/sequence.hpp"
#include "fields/since.hpp"
#include "fields/tally.hpp"
#include "fields/text.hpp"
#include "fields/when.hpp"
#include "gradient.hpp"
#include "path.hpp"
#include "shapes.hpp"

#include "dxfio/entities/hatch/hatch.hpp"
#include "dxfio/version.hpp"

#include <string_view>
#include <tuple>

namespace DxfIO
{
  template <>
  inline constexpr auto Shapes<Entities::Stripe> = std::tuple{
      Fields::Field{.Code = 53, .Member = &Entities::Stripe::Angle},
      Fields::Field{.Code = 43, .Member = &Entities::Stripe::X},
      Fields::Field{.Code = 44, .Member = &Entities::Stripe::Y},
      Fields::Field{.Code = 45, .Member = &Entities::Stripe::Run},
      Fields::Field{.Code = 46, .Member = &Entities::Stripe::Rise},
      Fields::Tally{.Code = 79, .Member = &Entities::Stripe::Dashes},
      Fields::Many{.Code = 49, .Member = &Entities::Stripe::Dashes},
  };

  namespace Entities
  {
    inline auto Patterned(const Fill &one) -> bool
    {
      return one.Solid == 0;
    }

    inline constexpr std::tuple Design{
        Fields::Field{.Code = 52, .Member = &Fill::Angle},
        Fields::Field{.Code = 41, .Member = &Fill::Scale},
        Fields::Field{.Code = 77, .Member = &Fill::Doubled},
        Fields::Count{.Code = 78, .Member = &Fill::Stripes},
        Fields::Each{
            .Opener = 53,
            .Member = &Fill::Stripes,
            .Fields = Shapes<Stripe>,
        },
    };

    inline constexpr std::tuple Shading{
        Fields::Maybe{
            .Opener = 450,
            .Member = &Fill::Gradient,
            .Fields = Shapes<Gradient>,
        },
    };
  } // namespace Entities

  template <>
  inline constexpr auto Shapes<Entities::Fill> = std::tuple{
      Fields::Sequence{.Marker = "AcDbHatch"},
      Fields::Field{.Code = 10, .Member = &Entities::Fill::Elevation},
      Fields::Field{.Code = 210, .Member = &Entities::Fill::Normal},
      Fields::Field{.Code = 2, .Member = &Entities::Fill::Name},
      Fields::Field{.Code = 70, .Member = &Entities::Fill::Solid},
      Fields::Field{.Code = 71, .Member = &Entities::Fill::Associative},
      Fields::Count{.Code = 91, .Member = &Entities::Fill::Paths},
      Fields::Each{
          .Opener = 92,
          .Member = &Entities::Fill::Paths,
          .Fields = Shapes<Entities::Path>,
      },
      Fields::Field{.Code = 75, .Member = &Entities::Fill::Style},
      Fields::Field{.Code = 76, .Member = &Entities::Fill::Kind},
      Fields::When{.Test = Entities::Patterned, .Fields = Entities::Design},
      Fields::Field{.Code = 47, .Member = &Entities::Fill::Pixel},
      Fields::Count{.Code = 98, .Member = &Entities::Fill::Seeds},
      Fields::Each{
          .Opener = 10,
          .Member = &Entities::Fill::Seeds,
          .Fields = Shapes<Entities::Edges::Place>,
      },
      Fields::Since{.Release = Version::R2004, .Fields = Entities::Shading},
  };

  template <>
  inline constexpr auto Shapes<Entities::Hatch> = std::tuple{
      Entities::Part<Entities::Hatch>,
      Fields::Part{
          .Member = &Entities::Hatch::Fill,
          .Fields = Shapes<Entities::Fill>,
      },
  };

  template <>
  inline constexpr std::string_view Type<Entities::Hatch> = "HATCH";
} // namespace DxfIO

#endif /* DXFIO_ENTITIES_SHAPES_HATCH_HATCH_HPP */
