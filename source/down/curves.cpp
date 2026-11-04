#include "drawn.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <numbers>
#include <span>
#include <utility>
#include <vector>

namespace DxfIO::Down
{
namespace
{
constexpr int Spatial = 8;

constexpr int Pointed = 32;

constexpr double Whole = 2 * std::numbers::pi;

constexpr std::size_t Around = 128;

constexpr std::size_t Spanned = 16;

auto Plus(const Vector &one, const Vector &other) -> Vector
{
  return Vector{.X = one.X + other.X, .Y = one.Y + other.Y, .Z = one.Z + other.Z};
}

auto Times(const Vector &one, double by) -> Vector
{
  return Vector{.X = one.X * by, .Y = one.Y * by, .Z = one.Z * by};
}

auto Length(const Vector &one) -> double
{
  return std::hypot(one.X, one.Y, one.Z);
}

auto Joined(const Entities::Record &record, std::span<const Vector> points, Seed &handles) -> Entities::Polyline
{
  Entities::Polyline out{};
  out.Record = record;
  out.Flags = Spatial;
  for (const Vector &point : points)
  {
    Entities::Vertex child{};
    child.Record = Child(record, handles);
    child.Position = point;
    child.Flags = Pointed;
    out.Vertices.Children.push_back(std::move(child));
  }
  out.Vertices.Closing = Boxed{Entities::Seqend{.Record = Child(record, handles)}};
  return out;
}

struct Homogeneous
{
  Vector Point{};
  double Weight{1};
};

auto Span(const Entities::Curve &one, double at) -> std::size_t
{
  const auto degree = static_cast<std::size_t>(one.Degree);
  const auto above = static_cast<std::size_t>(std::ranges::upper_bound(one.Knots, at) - one.Knots.begin());
  std::size_t span = std::min(std::max(above, degree + 1) - 1, one.Poles.size() - 1);
  while (one.Knots.at(span) == one.Knots.at(span + 1))
  {
    --span;
  }
  return span;
}

auto Evaluated(const Entities::Curve &one, double at, std::vector<Homogeneous> &local) -> Vector
{
  const auto degree = static_cast<std::size_t>(one.Degree);
  const std::size_t span = Span(one, at);
  local.clear();
  for (std::size_t index = 0; index <= degree; ++index)
  {
    const std::size_t pole = span - degree + index;
    const double weight = pole < one.Weights.size() ? one.Weights.at(pole) : 1.0;
    local.push_back(Homogeneous{.Point = Times(one.Poles.at(pole), weight), .Weight = weight});
  }
  for (std::size_t level = 1; level <= degree; ++level)
  {
    for (std::size_t index = degree; index >= level; --index)
    {
      const std::size_t knot = span - degree + index;
      const double low = one.Knots.at(knot);
      const double high = one.Knots.at(knot + degree + 1 - level);
      const double part = (at - low) / (high - low);
      local.at(index).Point = Plus(Times(local.at(index - 1).Point, 1 - part), Times(local.at(index).Point, part));
      local.at(index).Weight = (local.at(index - 1).Weight * (1 - part)) + (local.at(index).Weight * part);
    }
  }
  const Homogeneous &end = local.at(degree);
  return end.Weight != 0 ? Times(end.Point, 1 / end.Weight) : end.Point;
}

auto Valid(const Entities::Curve &one) -> bool
{
  return one.Degree >= 1 && one.Poles.size() > static_cast<std::size_t>(one.Degree) &&
         one.Knots.size() == one.Poles.size() + static_cast<std::size_t>(one.Degree) + 1 &&
         std::ranges::is_sorted(one.Knots) &&
         one.Knots.at(static_cast<std::size_t>(one.Degree)) < one.Knots.at(one.Poles.size());
}

auto Sampled(const Entities::Curve &one) -> std::vector<Vector>
{
  if (one.Poles.empty())
  {
    return one.Fits;
  }
  if (!Valid(one))
  {
    return one.Poles;
  }
  const auto degree = static_cast<std::size_t>(one.Degree);
  const double first = one.Knots.at(degree);
  const double last = one.Knots.at(one.Poles.size());
  const std::size_t steps = std::max(Around, Spanned * (one.Poles.size() - degree));
  std::vector<Vector> out;
  out.reserve(steps + 1);
  std::vector<Homogeneous> local;
  local.reserve(degree + 1);
  for (std::size_t step = 0; step <= steps; ++step)
  {
    const double at = first + ((last - first) * static_cast<double>(step) / static_cast<double>(steps));
    out.push_back(Evaluated(one, at, local));
  }
  return out;
}

auto Swept(double apart) -> double
{
  if (!std::isfinite(apart))
  {
    return Whole;
  }
  if (apart > 0)
  {
    return std::min(apart, Whole);
  }
  return std::fmod(apart, Whole) + Whole;
}
} // namespace

auto Child(const Entities::Record &parent, Seed &handles) -> Entities::Record
{
  Entities::Record out = parent;
  out.Base = {};
  out.Base.Self = handles.Take();
  out.Base.Owner = parent.Base.Self;
  return out;
}

auto Traced(const Entities::Ellipse &one, Seed &handles) -> Entities::Polyline
{
  const double radius = Length(one.Extrusion);
  const Vector normal = radius > 0 ? Times(one.Extrusion, 1 / radius) : Up;
  const Vector minor = Times(Cross(normal, one.Axis), one.Ratio);
  const double start = std::isfinite(one.Start) ? std::fmod(one.Start, Whole) : 0;
  const double sweep = Swept(one.End - one.Start);
  const auto steps = std::max<std::size_t>(2, static_cast<std::size_t>(std::ceil(Around * sweep / Whole)));
  std::vector<Vector> points;
  for (std::size_t step = 0; step <= steps; ++step)
  {
    const double at = start + (sweep * static_cast<double>(step) / static_cast<double>(steps));
    points.push_back(Plus(one.Center, Plus(Times(one.Axis, std::cos(at)), Times(minor, std::sin(at)))));
  }
  return Joined(one.Record, points, handles);
}

auto Traced(const Entities::Spline &one, Seed &handles) -> Entities::Polyline
{
  return Joined(one.Record, Sampled(*one.Curve), handles);
}
} // namespace DxfIO::Down
