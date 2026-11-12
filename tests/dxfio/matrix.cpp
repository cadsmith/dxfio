#include "dxfio/matrix.hpp"

#include <doctest/doctest.h>

namespace DxfIO
{
  TEST_CASE("matrix: the identity unless given, compared cell by cell")
  {
    CHECK(Matrix{} == Identity);
    Matrix scaled;
    scaled.Cells.front() = 2;
    CHECK_FALSE(scaled          == Identity);
    CHECK(Identity.Cells.back() == 1);
  }
} // namespace DxfIO
