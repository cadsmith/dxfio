#include "entities/shapes/ray.hpp"

#include "../kind.hpp"
#include "drawn.hpp"

namespace DxfIO::Entities
{
TEST_CASE("ray: where it starts and its direction, as given")
{
  const Ray taken = Replayed<Ray>(Drawn("2F3"));
  CHECK(taken.Record.Color == 1);
  CHECK(taken.Start == Vector{});
  CHECK(taken.Direction.X == 0.7071067811865476);
}

TEST_CASE("ray: a record giving no direction takes the x axis")
{
  const Ray taken = Read<Ray>("  0\r\nRAY\r\n100\r\nAcDbEntity\r\n  8\r\n0\r\n"
                              "100\r\nAcDbRay\r\n");
  CHECK(taken.Direction == Right);
}
} // namespace DxfIO::Entities
