#ifndef DXFIO_ENTITIES_HATCH_HATCH_HPP
#define DXFIO_ENTITIES_HATCH_HATCH_HPP

#include "dxfio/api.hpp"
#include "dxfio/boxed.hpp"
#include "dxfio/entities/hatch/edge.hpp"
#include "dxfio/entities/hatch/gradient.hpp"
#include "dxfio/entities/hatch/path.hpp"
#include "dxfio/entities/record.hpp"
#include "dxfio/vector.hpp"

#include <optional>
#include <string>
#include <vector>

namespace DxfIO::Entities
{
/// A line family of a hatch pattern: a line repeated across the hatch, dashed or not.
struct DXFIO_API Stripe
{
  /// The angle of the lines, in degrees. Group 53.
  double Angle{};
  /// The X of a point the first line passes through. Group 43.
  double X{};
  /// The Y of a point the first line passes through. Group 44.
  double Y{};
  /// The X of the offset from one line to the next. Group 45.
  double Run{};
  /// The Y of the offset from one line to the next. Group 46.
  double Rise{};
  /// The lengths of the dashes, positive for a dash and negative for a gap; group 79 counts them. Group 49.
  std::vector<double> Dashes;

  auto operator==(const Stripe &) const -> bool = default;
};

/// The AcDbHatch subclass of a HATCH: its boundary paths, its pattern or gradient, and its seed points.
struct DXFIO_API Fill
{
  /// A point whose Z is the hatch's elevation; its X and Y are 0. Groups 10, 20, 30.
  Vector Elevation{};
  /// The normal of the plane the entity is drawn in. Groups 210, 220, 230.
  Vector Normal{Up};
  /// The name of the pattern, such as ANSI31 or SOLID; SOLID for a new hatch. Group 2.
  std::string Name{"SOLID"};
  /// 1 a solid fill, as a new hatch is, 0 a pattern. Group 70.
  int Solid{1};
  /// 1 when the hatch follows the entities its boundary was made from. Group 71.
  int Associative{};
  /// The boundary paths; group 91 counts them.
  std::vector<Path> Paths;
  /// Which areas are hatched: 0 odd parity, 1 the outermost only, 2 all of it. Group 75.
  int Style{};
  /// Where the pattern comes from: 0 user-defined, 1 predefined, as SOLID is, 2 custom. Group 76.
  int Kind{1};
  /// The angle of the pattern, in degrees, for a pattern fill. Group 52.
  double Angle{};
  /// The scale or spacing of the pattern, for a pattern fill. Group 41.
  double Scale{1};
  /// 1 when a user-defined pattern is crosshatched, for a pattern fill. Group 77.
  int Doubled{};
  /// The line families of the pattern, for a pattern fill; group 78 counts them.
  std::vector<Stripe> Stripes;
  /// The pixel size AutoCAD computed the hatch with. Group 47.
  std::optional<double> Pixel;
  /// The seed points, in the hatch's own coordinate system; group 98 counts them.
  std::vector<Edges::Place> Seeds;
  /// The gradient, when the hatch has one, opened by group 450. From R2004.
  std::optional<Entities::Gradient> Gradient;

  auto operator==(const Fill &) const -> bool = default;
};

/// A HATCH: an area filled with a pattern, a solid color or a gradient.
struct DXFIO_API Hatch
{
  /// The layer, linetype, color and the rest every entity carries.
  Entities::Record Record{};
  /// The boundaries and how they are filled, from the AcDbHatch subclass.
  Boxed<Entities::Fill> Fill;

  auto operator==(const Hatch &) const -> bool = default;
};
} // namespace DxfIO::Entities

#endif /* DXFIO_ENTITIES_HATCH_HATCH_HPP */
