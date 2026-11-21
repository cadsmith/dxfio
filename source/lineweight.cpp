#include "dxfio/lineweight.hpp"

#include <algorithm>
#include <array>
#include <optional>
#include <utility>

namespace DxfIO
{
  namespace
  {
    constexpr std::array<int, 24> Hundredths{0,  5,  9,  13, 15, 18,  20,  25,  30,  35,  40,  50,
                                             53, 60, 70, 80, 90, 100, 106, 120, 140, 158, 200, 211};

    constexpr double PerMillimeter = 100.0;
  } // namespace

  auto MillimetersOf(Lineweight lineweight) -> std::optional<double>
  {
    const int width = std::to_underlying(lineweight);
    if (!std::ranges::contains(Hundredths, width))
    {
      return std::nullopt;
    }
    return width / PerMillimeter;
  }
} // namespace DxfIO
