#ifndef DXFIO_ENTITIES_SHAPES_SPLINE_HPP
#define DXFIO_ENTITIES_SHAPES_SPLINE_HPP

#include "fields/custom.hpp"
#include "fields/field.hpp"
#include "fields/many.hpp"
#include "fields/numbers.hpp"
#include "fields/optional.hpp"
#include "fields/part.hpp"
#include "fields/points.hpp"
#include "fields/subclass.hpp"
#include "fields/tally.hpp"
#include "record.hpp"
#include "shapes.hpp"

#include "dxfio/entities/spline.hpp"
#include "dxfio/vector.hpp"

#include <algorithm>
#include <cstddef>
#include <expected>
#include <string_view>
#include <tuple>

namespace DxfIO
{
  namespace Entities
  {
    inline constexpr int Polar = 10;

    inline constexpr int Weighted = 41;

    inline void Pointed(Curve &one, Fields::Walk &in)
    {
      Take(Fields::Many{.Code = Polar, .Member = &Curve::Poles}, one, in);
      Take(Fields::Many{.Code = Weighted, .Member = &Curve::Weights}, one, in);
    }

    inline auto Pointing(const Curve &one, Fields::Writing &to) -> std::expected<void, Failure>
    {
      const std::size_t            most = std::max(one.Poles.size(), one.Weights.size());
      std::expected<void, Failure> done{};
      for (std::size_t at = 0; at < most && done; ++at)
      {
        if (at < one.Poles.size())
        {
          done = Fields::Value<Vector>::Write(to, Polar, one.Poles.at(at));
        }
        if (done && at < one.Weights.size())
        {
          done = Fields::Value<double>::Write(to, Weighted, one.Weights.at(at));
        }
      }
      return done;
    }
  } // namespace Entities

  template <>
  inline constexpr auto Shapes<Entities::Curve> = std::tuple{
      Fields::Subclass{.Marker = "AcDbSpline"},
      Fields::Field{.Code = 210, .Member = &Entities::Curve::Normal},
      Fields::Field{.Code = 70, .Member = &Entities::Curve::Flags},
      Fields::Field{.Code = 71, .Member = &Entities::Curve::Degree},
      Fields::Tally{.Code = 72, .Member = &Entities::Curve::Knots},
      Fields::Tally{.Code = 73, .Member = &Entities::Curve::Poles},
      Fields::Tally{.Code = 74, .Member = &Entities::Curve::Fits},
      Fields::Field{.Code = 42, .Member = &Entities::Curve::Knotted},
      Fields::Field{.Code = 43, .Member = &Entities::Curve::Framed},
      Fields::Field{.Code = 44, .Member = &Entities::Curve::Fitted},
      Fields::Field{.Code = 12, .Member = &Entities::Curve::Start},
      Fields::Field{.Code = 13, .Member = &Entities::Curve::End},
      Fields::Many{.Code = 40, .Member = &Entities::Curve::Knots},
      Fields::Custom<Entities::Curve>{
          .Read  = Entities::Pointed,
          .Write = Entities::Pointing,
      },
      Fields::Many{.Code = 11, .Member = &Entities::Curve::Fits},
  };

  template <>
  inline constexpr auto Shapes<Entities::Spline> = std::tuple{
      Entities::Part<Entities::Spline>,
      Fields::Part{
          .Member = &Entities::Spline::Curve,
          .Fields = Shapes<Entities::Curve>,
      },
  };

  template <>
  inline constexpr std::string_view Type<Entities::Spline> = "SPLINE";
} // namespace DxfIO

#endif /* DXFIO_ENTITIES_SHAPES_SPLINE_HPP */
