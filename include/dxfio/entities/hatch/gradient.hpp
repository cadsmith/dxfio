#ifndef DXFIO_ENTITIES_HATCH_GRADIENT_HPP
#define DXFIO_ENTITIES_HATCH_GRADIENT_HPP

#include "dxfio/api.hpp"

#include <optional>
#include <string>
#include <vector>

namespace DxfIO::Entities
{
/// A color of a gradient, and where along the gradient it lies.
struct DXFIO_API Stop
{
  /// Where the color lies along the gradient, 0 to 1. Group 463.
  double Place{};
  /// The AutoCAD Color Index of the color, when it has one. Group 63.
  std::optional<int> Color;
  /// The true color, as 0xRRGGBB, when it has one. Group 421.
  std::optional<int> Rgb;

  auto operator==(const Stop &) const -> bool = default;
};

/// The gradient a HATCH is filled with, from R2004.
struct DXFIO_API Gradient
{
  /// 0 a solid fill, 1 a gradient. Group 450.
  int Shown{};
  /// Reserved by AutoCAD. Group 451.
  int Reserved{};
  /// The rotation of the gradient, in radians. Group 460.
  double Angle{};
  /// How far the gradient is shifted off its center, 0 to 1. Group 461.
  double Shift{};
  /// 1 when the gradient runs from one color, 0 when from two. Group 452.
  int Single{};
  /// How far a gradient of one color is tinted, 0 to 1. Group 462.
  double Tint{};
  /// The colors; group 453 counts them.
  std::vector<Stop> Stops;
  /// The name of the gradient, such as LINEAR or SPHERICAL. Group 470.
  std::string Name;

  auto operator==(const Gradient &) const -> bool = default;
};
} // namespace DxfIO::Entities

#endif /* DXFIO_ENTITIES_HATCH_GRADIENT_HPP */
