#include "drawn.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <optional>
#include <string_view>
#include <utility>
#include <variant>

namespace DxfIO::Down
{
  namespace
  {
    constexpr double Reach = 1000;

    auto Point(const Document &from, std::string_view name) -> std::optional<Vector>
    {
      const auto found = std::ranges::find(from.Header, name, &Header::Variable::Name);
      if (found == from.Header.end())
      {
        return std::nullopt;
      }
      std::array<double, 3> axes{};
      std::size_t           read = 0;
      for (const Group &one : found->Values)
      {
        const auto *held = std::get_if<double>(&one.Value);
        if (held != nullptr && read < axes.size())
        {
          axes.at(read++) = *held;
        }
      }
      if (read < 2)
      {
        return std::nullopt;
      }
      return Vector{.X = axes.at(0), .Y = axes.at(1), .Z = axes.at(2)};
    }

    auto Around(const Vector &at) -> Extents
    {
      return Extents{
          .Low  = Vector{.X = at.X - Reach, .Y = at.Y - Reach, .Z = at.Z - Reach},
          .High = Vector{.X = at.X + Reach, .Y = at.Y + Reach, .Z = at.Z + Reach},
      };
    }

    auto Valid(const Extents &box) -> bool
    {
      return box.Low.X < box.High.X && box.Low.Y < box.High.Y && box.Low.Z <= box.High.Z;
    }

    struct Axis
    {
      double Base{};
      double Step{};
      double Low{};
      double High{};
    };

    struct Infinite
    {
      Vector Start{};
      Vector Direction{};
      double From{};
    };

    auto Slab(const Axis &along, std::pair<double, double> &range) -> void
    {
      if (along.Step == 0)
      {
        return;
      }
      double near = (along.Low - along.Base) / along.Step;
      double far  = (along.High - along.Base) / along.Step;
      if (near > far)
      {
        std::swap(near, far);
      }
      range.first  = std::max(range.first, near);
      range.second = std::min(range.second, far);
    }

    auto Cut(const Infinite &one, const Extents &box) -> Entities::Line
    {
      const Vector &start     = one.Start;
      const Vector &direction = one.Direction;
      const double  from      = one.From;
      const Extents used      = Valid(box) ? box : Around(start);
      std::pair     range{from, std::numeric_limits<double>::infinity()};
      Slab(Axis{.Base = start.X, .Step = direction.X, .Low = used.Low.X, .High = used.High.X},
           range);
      Slab(Axis{.Base = start.Y, .Step = direction.Y, .Low = used.Low.Y, .High = used.High.Y},
           range);
      Slab(Axis{.Base = start.Z, .Step = direction.Z, .Low = used.Low.Z, .High = used.High.Z},
           range);
      if (std::isfinite(from))
      {
        range.first = from;
      }
      if (!(range.first < range.second)
          || !std::isfinite(range.first)
          || !std::isfinite(range.second))
      {
        range = {std::isfinite(from) ? from : -Reach, Reach};
      }
      const auto along = [&](double at) -> Vector
      {
        return Vector{
            .X = start.X + (direction.X * at),
            .Y = start.Y + (direction.Y * at),
            .Z = start.Z + (direction.Z * at),
        };
      };
      Entities::Line out{};
      out.Start = along(range.first);
      out.End   = along(range.second);
      return out;
    }
  } // namespace

  auto Bounded(const Document &from) -> Extents
  {
    const std::optional<Vector> low  = Point(from, "$EXTMIN");
    const std::optional<Vector> high = Point(from, "$EXTMAX");
    if (!low || !high)
    {
      return {};
    }
    return Extents{.Low = *low, .High = *high};
  }

  auto Clipped(const Entities::Ray &one, const Extents &box) -> Entities::Line
  {
    Entities::Line out =
        Cut(Infinite{.Start = one.Start, .Direction = one.Direction, .From = 0}, box);
    out.Record = one.Record;
    return out;
  }

  auto Clipped(const Entities::XLine &one, const Extents &box) -> Entities::Line
  {
    Entities::Line out = Cut(Infinite{.Start     = one.Start,
                                      .Direction = one.Direction,
                                      .From      = -std::numeric_limits<double>::infinity()},
                             box);
    out.Record         = one.Record;
    return out;
  }
} // namespace DxfIO::Down
