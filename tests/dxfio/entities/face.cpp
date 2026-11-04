#include "entities/shapes/face.hpp"

#include "../kind.hpp"
#include "drawn.hpp"

namespace DxfIO::Entities
{
TEST_CASE("face: its corners each at their own height")
{
  const Face3d taken = Replayed<Face3d>(Drawn("2FA"));
  CHECK(taken.Corners.at(3) == Vector{.X = 0, .Y = 10, .Z = 3});
  CHECK(taken.Flags == 0);
}

TEST_CASE("face: its hidden edges on 70, written only when any is")
{
  const Face3d taken = Replayed<Face3d>(Drawn("2FB"));
  CHECK(taken.Flags == 5);
}

TEST_CASE("face: a triangle's fourth corner its third")
{
  const Face3d taken = Replayed<Face3d>(Drawn("2FC"));
  CHECK(taken.Corners.at(3) == taken.Corners.at(2));
}

TEST_CASE("face: R12's, without subclasses")
{
  const Face3d taken = Replayed<Face3d>(Drawn("2FB", Version::R12), Version::R12);
  CHECK(taken.Flags == 5);
}
} // namespace DxfIO::Entities
