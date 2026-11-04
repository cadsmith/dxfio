#include "seed.hpp"

#include <doctest/doctest.h>

#include <cstdint>
#include <limits>
#include <vector>

namespace DxfIO
{
TEST_CASE("seed: handles taken one past the last, the next one kept")
{
  Seed handles;
  CHECK(handles.Take() == Handle{.Value = 1});
  CHECK(handles.Take() == Handle{.Value = 2});
  CHECK(handles.Next() == Handle{.Value = 3});
  Seed later{Handle{.Value = 0x20}};
  CHECK(later.Take() == Handle{.Value = 0x20});
  CHECK(later.Next() == Handle{.Value = 0x21});
}

TEST_CASE("seed: the last handle there is taken once, none after it")
{
  Seed last{Handle{std::numeric_limits<std::uint64_t>::max()}};
  CHECK_FALSE(last.Take());
  CHECK_FALSE(last.Next());
  CHECK_FALSE(last.Take());
}

TEST_CASE("seed: no handle to start from stays none")
{
  Seed none{Handle{}};
  CHECK_FALSE(none.Take());
  CHECK_FALSE(none.Next());
}

TEST_CASE("seed: the lowest handles none of those in use is, one after the other")
{
  Seed gaps{std::vector<Handle>{{.Value = 7}, {.Value = 3}, {}, {.Value = 1}, {.Value = 3}, {.Value = 4}}};
  CHECK(gaps.Take() == Handle{.Value = 2});
  CHECK(gaps.Take() == Handle{.Value = 5});
  CHECK(gaps.Take() == Handle{.Value = 6});
  CHECK(gaps.Take() == Handle{.Value = 8});
  CHECK(gaps.Take() == Handle{.Value = 9});
  CHECK(Seed{std::vector<Handle>{}}.Take() == Handle{.Value = 1});
}
} // namespace DxfIO
