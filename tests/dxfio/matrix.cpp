#include "dxfio/matrix.hpp"

#include <doctest/doctest.h>

namespace DxfIO
{
  TEST_CASE("matrix: the identity has ones on its diagonal, compared cell by cell")
  {
    Matrix scaled  = Identity;
    scaled.front() = 2;
    CHECK_FALSE(scaled    == Identity);
    CHECK(Identity.at(5)  == 1);
    CHECK(Identity.at(1)  == 0);
    CHECK(Identity.back() == 1);
  }
} // namespace DxfIO
