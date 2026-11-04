#include "dxfio/base.hpp"

#include <doctest/doctest.h>

#include <string>

namespace DxfIO
{
TEST_CASE("base: what every record carries, none of it until given")
{
  const Base fresh{};
  CHECK_FALSE(fresh.Self);
  CHECK_FALSE(fresh.Owner);
  CHECK_FALSE(fresh.Dictionary);
  CHECK(fresh.Reactors.empty());
  CHECK(fresh.Extra.empty());
}

TEST_CASE("base: compared by all it holds")
{
  Base one{};
  one.Self = Handle{0x1f};
  one.Owner = Handle{0x1};
  Base other = one;
  CHECK(one == other);
  other.Extra.push_back(Group{.Code = 999, .Value = std::string{"x"}});
  CHECK_FALSE(one == other);
}
} // namespace DxfIO
