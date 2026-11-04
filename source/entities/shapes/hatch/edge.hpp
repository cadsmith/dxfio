#ifndef DXFIO_ENTITIES_SHAPES_HATCH_EDGE_HPP
#define DXFIO_ENTITIES_SHAPES_HATCH_EDGE_HPP

#include "../../../variants.hpp"
#include "fields/count.hpp"
#include "fields/custom.hpp"
#include "fields/each.hpp"
#include "fields/field.hpp"
#include "fields/flat.hpp"
#include "fields/many.hpp"
#include "fields/numbers.hpp"
#include "fields/optional.hpp"
#include "fields/since.hpp"
#include "fields/tally.hpp"
#include "fields/when.hpp"
#include "shapes.hpp"

#include "dxfio/boxed.hpp"
#include "dxfio/entities/hatch/edge.hpp"
#include "dxfio/failure.hpp"
#include "dxfio/version.hpp"

#include <cstddef>
#include <expected>
#include <tuple>
#include <utility>
#include <variant>

namespace DxfIO
{
template <>
inline constexpr auto Shapes<Entities::Edges::Node> = std::tuple{
    Fields::Flat{.Code = 10, .Member = &Entities::Edges::Node::Position},
    Fields::Field{.Code = 42, .Member = &Entities::Edges::Node::Weight},
};

template <>
inline constexpr auto Shapes<Entities::Edges::Place> = std::tuple{
    Fields::Flat{.Code = 10, .Member = &Entities::Edges::Place::Position},
};

template <>
inline constexpr auto Shapes<Entities::Edges::Line> = std::tuple{
    Fields::Flat{.Code = 10, .Member = &Entities::Edges::Line::Start},
    Fields::Flat{.Code = 11, .Member = &Entities::Edges::Line::End},
};

template <>
inline constexpr auto Shapes<Entities::Edges::Arc> = std::tuple{
    Fields::Flat{.Code = 10, .Member = &Entities::Edges::Arc::Center},
    Fields::Field{.Code = 40, .Member = &Entities::Edges::Arc::Radius},
    Fields::Field{.Code = 50, .Member = &Entities::Edges::Arc::Start},
    Fields::Field{.Code = 51, .Member = &Entities::Edges::Arc::End},
    Fields::Field{.Code = 73, .Member = &Entities::Edges::Arc::Counter},
};

template <>
inline constexpr auto Shapes<Entities::Edges::Ellipse> = std::tuple{
    Fields::Flat{.Code = 10, .Member = &Entities::Edges::Ellipse::Center},
    Fields::Flat{.Code = 11, .Member = &Entities::Edges::Ellipse::Major},
    Fields::Field{.Code = 40, .Member = &Entities::Edges::Ellipse::Ratio},
    Fields::Field{.Code = 50, .Member = &Entities::Edges::Ellipse::Start},
    Fields::Field{.Code = 51, .Member = &Entities::Edges::Ellipse::End},
    Fields::Field{.Code = 73, .Member = &Entities::Edges::Ellipse::Counter},
};

namespace Entities::Edges
{
inline constexpr std::tuple Fit{
    Fields::Flat{.Code = 11, .Member = &Place::Position},
};

inline auto Fitted(const Spline &one) -> bool
{
  return !one.Fits.empty();
}

inline constexpr std::tuple Ends{
    Fields::Flat{.Code = 12, .Member = &Spline::Start},
    Fields::Flat{.Code = 13, .Member = &Spline::End},
};

inline constexpr std::tuple Later{
    Fields::Count{.Code = 97, .Member = &Spline::Fits},
    Fields::Each{.Opener = 11, .Member = &Spline::Fits, .Fields = Fit},
    Fields::When{.Test = Fitted, .Fields = Ends},
};
} // namespace Entities::Edges

template <>
inline constexpr auto Shapes<Entities::Edges::Spline> = std::tuple{
    Fields::Field{.Code = 94, .Member = &Entities::Edges::Spline::Degree},
    Fields::Field{.Code = 73, .Member = &Entities::Edges::Spline::Rational},
    Fields::Field{.Code = 74, .Member = &Entities::Edges::Spline::Periodic},
    Fields::Tally{.Code = 95, .Member = &Entities::Edges::Spline::Knots},
    Fields::Count{.Code = 96, .Member = &Entities::Edges::Spline::Poles},
    Fields::Many{.Code = 40, .Member = &Entities::Edges::Spline::Knots},
    Fields::Each{
        .Opener = 10,
        .Member = &Entities::Edges::Spline::Poles,
        .Fields = Shapes<Entities::Edges::Node>,
    },
    Fields::Since{.Release = Version::R2010, .Fields = Entities::Edges::Later},
};

namespace Entities::Edges
{
inline void Splined(Boxed<Spline> &one, Fields::Walk &in)
{
  Fields::Read(*one, in, Shapes<Spline>);
}

inline auto Splining(const Boxed<Spline> &one, Fields::Writing &to) -> std::expected<void, Failure>
{
  return Fields::Write(*one, to, Shapes<Spline>);
}
} // namespace Entities::Edges

template <>
inline constexpr auto Shapes<Boxed<Entities::Edges::Spline>> = std::tuple{
    Fields::Custom<Boxed<Entities::Edges::Spline>>{
        .Read = Entities::Edges::Splined,
        .Write = Entities::Edges::Splining,
    },
};

namespace Entities::Edges
{
inline constexpr int Kind = 72;

inline void Chosen(Edge &one, Fields::Walk &in)
{
  const int type = Fields::Value<int>::Read(in, Kind).value_or(0);
  if (type < 1 || std::cmp_greater(type, std::variant_size_v<Edge>))
  {
    in.Refuse();
    return;
  }
  Become(one, static_cast<std::size_t>(type - 1), in);
}

inline auto Choosing(const Edge &one, Fields::Writing &to) -> std::expected<void, Failure>
{
  const int type = static_cast<int>(one.index()) + 1;
  return Fields::Value<int>::Write(to, Kind, type)
      .and_then([&] -> std::expected<void, Failure> { return Spoken(one, to); });
}
} // namespace Entities::Edges

template <>
inline constexpr auto Shapes<Entities::Edges::Edge> = std::tuple{
    Fields::Custom<Entities::Edges::Edge>{
        .Read = Entities::Edges::Chosen,
        .Write = Entities::Edges::Choosing,
    },
};
} // namespace DxfIO

#endif /* DXFIO_ENTITIES_SHAPES_HATCH_EDGE_HPP */
