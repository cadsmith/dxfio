#include "tags/whole.hpp"

#include <doctest/doctest.h>

namespace DxfIO::Tags
{
TEST_CASE("whole: digits and their sign read, nothing else")
{
  CHECK(Whole("0") == 0);
  CHECK(Whole("-5") == -5);
  CHECK_FALSE(Whole(""));
  CHECK_FALSE(Whole("5a"));
  CHECK_FALSE(Whole("99999999999"));
}
} // namespace DxfIO::Tags
