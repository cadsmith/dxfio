#include "checks.hpp"
#include "walk.hpp"

#include "dxfio/entities/ellipse.hpp"
#include "dxfio/vector.hpp"

#include <cmath>
#include <format>
#include <numbers>
#include <variant>

namespace DxfIO::Audit
{
namespace
{
// The least ratio AutoCAD takes.
constexpr double Least = 1e-6;

constexpr double Quarter = std::numbers::pi / 2;

auto Cross(const Vector &one, const Vector &other) -> Vector
{
  return Vector{
      .X = (one.Y * other.Z) - (one.Z * other.Y),
      .Y = (one.Z * other.X) - (one.X * other.Z),
      .Z = (one.X * other.Y) - (one.Y * other.X),
  };
}

auto Length(const Vector &one) -> double
{
  return std::sqrt((one.X * one.X) + (one.Y * one.Y) + (one.Z * one.Z));
}

auto Scaled(const Vector &one, double by) -> Vector
{
  return Vector{.X = one.X * by, .Y = one.Y * by, .Z = one.Z * by};
}

auto Wrapped(double angle) -> double
{
  const double turned = std::fmod(angle, Entities::Turn);
  return turned < 0 ? turned + Entities::Turn : turned;
}

// Whether a ratio is one AutoCAD takes: above 0, at most 1, and a number.
auto Fits(double ratio) -> bool
{
  return ratio > 0 && ratio <= 1;
}

// Makes the minor axis the major one when it is the longer: the same curve, a quarter turn back along it, with
// the ratio the other way round.
void Swapped(Entities::Ellipse &one)
{
  const Vector across = Cross(one.Extrusion, one.Axis);
  const double length = Length(across);
  one.Axis = Scaled(across, length == 0 ? 0 : Length(one.Axis) * one.Ratio / length);
  one.Ratio = 1 / one.Ratio;
  if (one.End - one.Start >= Entities::Turn)
  {
    return;
  }
  one.Start = Wrapped(one.Start - Quarter);
  one.End = Wrapped(one.End - Quarter);
  if (one.End <= one.Start)
  {
    one.End += Entities::Turn;
  }
}

template <class DocumentT>
void Checked(Auditor<DocumentT> &with, Part<DocumentT, Entities::Ellipse> &one)
{
  if (Fits(one.Ratio))
  {
    return;
  }
  with.Note(Severity::Error, std::format("ellipse ratio {} not between 0 and 1", one.Ratio), one.Record.Base.Self);
  if constexpr (Auditor<DocumentT>::Fix)
  {
    if (one.Ratio > 1)
    {
      Swapped(one);
      return;
    }
    one.Ratio = Least;
  }
}
} // namespace

// AutoCAD refuses a drawing with an ELLIPSE whose ratio of minor to major axis is not between 0 and 1: such a ratio
// is an error; above 1 the axes are swapped, which draws the same curve, else the ratio is raised to the least taken.
template <class DocumentT>
void Curved(Auditor<DocumentT> &with)
{
  Lists(*with.Drawing,
        [&with](auto &all) -> void
        {
          for (auto &one : all)
          {
            if (auto *ellipse = std::get_if<Entities::Ellipse>(&one))
            {
              Checked(with, *ellipse);
            }
          }
        });
}

template void Curved(Auditor<Document> &with);

template void Curved(Auditor<const Document> &with);
} // namespace DxfIO::Audit
