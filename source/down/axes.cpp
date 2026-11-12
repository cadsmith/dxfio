#include "drawn.hpp"

#include <cmath>

namespace DxfIO::Down
{
  namespace
  {
    constexpr double Near = 1.0 / 64;

    constexpr Vector North{.X = 0, .Y = 1, .Z = 0};

    auto Unit(const Vector &one) -> Vector
    {
      const double length = std::hypot(one.X, one.Y, one.Z);
      return length > 0 ? Vector{.X = one.X / length, .Y = one.Y / length, .Z = one.Z / length}
                        : Up;
    }
  } // namespace

  auto Cross(const Vector &one, const Vector &other) -> Vector
  {
    return Vector{
        .X = (one.Y * other.Z) - (one.Z * other.Y),
        .Y = (one.Z * other.X) - (one.X * other.Z),
        .Z = (one.X * other.Y) - (one.Y * other.X),
    };
  }

  auto Arbitrary(const Vector &normal) -> Axes
  {
    const Vector z     = Unit(normal);
    const bool   polar = std::abs(z.X) < Near && std::abs(z.Y) < Near;
    const Vector x     = Unit(Cross(polar ? North : Up, z));
    return Axes{.X = x, .Y = Unit(Cross(z, x)), .Z = z};
  }
} // namespace DxfIO::Down
