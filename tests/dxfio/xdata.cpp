#include "dxfio/xdata.hpp"

#include <doctest/doctest.h>

#include <string>

namespace DxfIO
{
  TEST_CASE("xdata: an application's name and its groups")
  {
    const Xdata acad{.Application = "ACAD", .Tags = {Tag{.Code = 1000, .Value = std::string{"x"}}}};
    CHECK(acad       == acad);
    CHECK_FALSE(acad == Xdata{.Application = "ACAD", .Tags = {}});
  }
} // namespace DxfIO
