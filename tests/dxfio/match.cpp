#include "match.hpp"

#include <doctest/doctest.h>

#include <string_view>
#include <variant>

namespace DxfIO
{
TEST_CASE("match: a visit calls the lambda of the alternative held")
{
  const auto kind = [](const std::variant<int, std::string_view> &held) -> int
  {
    return std::visit(Match{[](int) -> int { return 1; }, [](std::string_view) -> int { return 2; }}, held);
  };
  CHECK(kind(7) == 1);
  CHECK(kind(std::string_view{"walls"}) == 2);
}
} // namespace DxfIO
