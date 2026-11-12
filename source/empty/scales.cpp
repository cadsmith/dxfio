#include "scales.hpp"

#include <array>
#include <format>
#include <utility>

namespace DxfIO::Empty
{
  namespace
  {
    constexpr auto Ratios = std::to_array<std::pair<int, int>>({
        {1, 1},
        {1, 2},
        {1, 4},
        {1, 5},
        {1, 8},
        {1, 10},
        {1, 16},
        {1, 20},
        {1, 30},
        {1, 40},
        {1, 50},
        {1, 100},
        {2, 1},
        {4, 1},
        {8, 1},
        {10, 1},
        {100, 1},
    });
  } // namespace

  auto Scales(Seed &handles) -> std::vector<Objects::Scale>
  {
    std::vector<Objects::Scale> out;
    for (const auto &[paper, drawing] : Ratios)
    {
      Objects::Scale one{};
      one.Base.Self = handles.Take();
      one.Name      = std::format("{}:{}", paper, drawing);
      one.Paper     = paper;
      one.Drawing   = drawing;
      one.Unit      = paper == drawing;
      out.push_back(std::move(one));
    }
    return out;
  }
} // namespace DxfIO::Empty
