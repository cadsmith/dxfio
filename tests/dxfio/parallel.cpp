#include "parallel.hpp"

#include <doctest/doctest.h>

#include <atomic>
#include <cstddef>
#include <mutex>
#include <stdexcept>
#include <string>
#include <vector>

namespace DxfIO
{
TEST_CASE("parallel: the threads asked for, or the machine's, one at least")
{
  CHECK(Threads(3) == 3);
  CHECK(Threads(0) >= 1);
  CHECK(Threads(-2) == Threads(0));
  CHECK(Parts(2) == 8);
  CHECK(Parts(1) == 1);
}

TEST_CASE("parallel: items cut into parts of near equal size, none shorter than least but the last")
{
  CHECK(Parted(0, 4, 1).empty());
  CHECK(Parted(10, 4, 1) == std::vector{Range{0, 3}, Range{3, 6}, Range{6, 9}, Range{9, 10}});
  CHECK(Parted(10, 4, 8) == std::vector{Range{0, 10}});
  CHECK(Parted(1000, 32, 256) == std::vector{Range{0, 334}, Range{334, 668}, Range{668, 1000}});
  CHECK(Parted(5, 32, 0) == std::vector{Range{0, 1}, Range{1, 2}, Range{2, 3}, Range{3, 4}, Range{4, 5}});
}

TEST_CASE("parallel: every job runs once, in order on one thread, on several when asked")
{
  std::vector<int> order;
  Spread(5, 1, [&order](std::size_t index) -> void { order.push_back(static_cast<int>(index)); });
  CHECK(order == std::vector{0, 1, 2, 3, 4});
  std::vector<std::atomic<int>> seen(100);
  Spread(100, 4, [&seen](std::size_t index) -> void { ++seen.at(index); });
  for (const std::atomic<int> &one : seen)
  {
    CHECK(one == 1);
  }
  int none = 0;
  Spread(0, 4, [&none](std::size_t) -> void { ++none; });
  CHECK(none == 0);
}

TEST_CASE("parallel: an exception a job throws reaches the caller once every thread has stopped")
{
  std::atomic<int> ran{0};
  const auto failing = [&ran](std::size_t index) -> void
  {
    ++ran;
    if (index == 7)
    {
      throw std::runtime_error{"seven"};
    }
  };
  CHECK_THROWS_WITH_AS(Spread(40, 4, failing), "seven", std::runtime_error);
  CHECK(ran >= 1);
  CHECK_THROWS_WITH_AS(Spread(40, 1, failing), "seven", std::runtime_error);
  const auto always = [](std::size_t index) -> void
  {
    throw std::runtime_error{std::to_string(index)};
  };
  CHECK_THROWS_AS(Spread(40, 4, always), std::runtime_error);
}
} // namespace DxfIO
