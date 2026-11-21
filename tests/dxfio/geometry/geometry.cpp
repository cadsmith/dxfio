#include "../disk.hpp"

#include "dxfio/entities/circle.hpp"
#include "dxfio/geometry/coordinates.hpp"
#include "dxfio/geometry/transform.hpp"
#include "dxfio/load.hpp"

#include <doctest/doctest.h>

#include <cmath>
#include <variant>

namespace DxfIO::Geometry
{
  namespace
  {
    auto Near(Vector one, Vector other) -> bool
    {
      return std::abs(one.X - other.X) < 1e-9
          && std::abs(one.Y - other.Y) < 1e-9
          && std::abs(one.Z - other.Z) < 1e-9;
    }
  } // namespace

  TEST_CASE(
      "geometry: a circle AutoCAD drew in a turned UCS lies at its elevation along its normal")
  {
    const auto loaded = Load(Acad("entities-r2000.dxf"));
    REQUIRE(loaded);
    const Entities::Circle *turned = nullptr;
    for (const Entity &one : loaded->Drawing.Entities)
    {
      const auto *circle = std::get_if<Entities::Circle>(&one);
      if (circle != nullptr && circle->Extrusion.Y < -0.5)
      {
        turned = circle;
      }
    }
    REQUIRE(turned != nullptr);
    const Entities::Circle &circle = *turned;
    /**
     *  The world point lies at the center's Z along the normal, and comes back as the center.
     */
    const Vector  world = ToWorld(circle.Center, circle.Extrusion);
    const Vector &n     = circle.Extrusion;
    CHECK((world.X * n.X) + (world.Y * n.Y) + (world.Z * n.Z) == doctest::Approx(circle.Center.Z));
    CHECK(Near(ToObject(world, circle.Extrusion), circle.Center));
  }

  TEST_CASE("geometry: the arbitrary axes of a normal along X, a mirrored one and none")
  {
    const Vector point{.X = 2, .Y = 3, .Z = 5};
    CHECK(Near(ToWorld(point, Vector{.X = 1, .Y = 0, .Z = 0}), Vector{.X = 5, .Y = 2, .Z = 3}));
    CHECK(Near(ToWorld(point, Vector{.X = 0, .Y = 0, .Z = -1}), Vector{.X = -2, .Y = 3, .Z = -5}));
    CHECK(Near(ToWorld(point, Vector{}), point));
    const Vector slanted{.X = 1, .Y = 2, .Z = 3};
    CHECK(Near(ToObject(ToWorld(point, slanted), slanted), point));
  }

  TEST_CASE(
      "geometry: an insert takes off the base point, scales, turns, moves and leaves its plane")
  {
    Blocks::Definition block;
    block.Block.Origin = Vector{.X = 1, .Y = 1, .Z = 0};
    Entities::Insert insert;
    insert.Position     = Vector{.X = 10, .Y = 20, .Z = 0};
    insert.Scale        = Vector{.X = 2, .Y = 3, .Z = 4};
    insert.Rotation     = 90;
    const Matrix placed = TransformOf(insert, block);
    CHECK(
        Near(Transform(placed, Vector{.X = 2, .Y = 1, .Z = 1}), Vector{.X = 10, .Y = 22, .Z = 4}));
    insert.Extrusion = Vector{.X = 0, .Y = 0, .Z = -1};
    CHECK(Near(Transform(TransformOf(insert, block), block.Block.Origin),
               Vector{.X = -10, .Y = 20, .Z = 0}));
    CHECK(Compose(Identity, placed) == placed);
  }
} // namespace DxfIO::Geometry
