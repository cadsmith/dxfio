#include "chained.hpp"

#include <doctest/doctest.h>

#include <array>
#include <expected>
#include <vector>

namespace DxfIO
{
namespace
{
auto Refused(int one) -> std::expected<void, Failure>
{
  if (one < 0)
  {
    return std::unexpected{Failure{.Line = 0, .What = "negative"}};
  }
  return {};
}
} // namespace

TEST_CASE("chained: each written in order, the first failure handed back")
{
  const std::array all{1, 2, -3, 4, -5};
  std::vector<int> seen;
  const auto done = Chained(all,
                            [&seen](int one) -> std::expected<void, Failure>
                            {
                              seen.push_back(one);
                              return Refused(one);
                            });
  REQUIRE_FALSE(done);
  CHECK(done.error().What == "negative");
  CHECK(seen == std::vector{1, 2, -3});
}

TEST_CASE("chained: every one written, or none to write")
{
  CHECK(Chained(std::array{1, 2}, Refused));
  CHECK(Chained(std::vector<int>{}, Refused));
}
} // namespace DxfIO
