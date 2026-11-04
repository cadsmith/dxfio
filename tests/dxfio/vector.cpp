#include "dxfio/vector.hpp"

#include <doctest/doctest.h>

namespace DxfIO
{
TEST_CASE("vector: compared by its three reals, z 0 unless given")
{
  CHECK(Vector{.X = 1, .Y = 2} == Vector{.X = 1, .Y = 2, .Z = 0});
  CHECK_FALSE(Vector{.X = 1} == Vector{.Y = 1});
  CHECK(Up == Vector{.X = 0, .Y = 0, .Z = 1});
}
} // namespace DxfIO
