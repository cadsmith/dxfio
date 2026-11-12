#include "header/rows.hpp"

#include "rows/axes.hpp"
#include "rows/dimensions.hpp"
#include "rows/drafting.hpp"
#include "rows/drawing.hpp"
#include "rows/scene.hpp"
#include "rows/space.hpp"
#include "rows/styles.hpp"

#include <algorithm>
#include <array>
#include <cstddef>

namespace DxfIO::Header::Rows
{
  namespace
  {
    template <std::size_t... Sizes>
    consteval auto Joined(const std::array<Row, Sizes> &...parts) -> std::array<Row, (Sizes + ...)>
    {
      std::array<Row, (Sizes + ...)> out{};
      auto                           at = out.begin();
      ((at = std::ranges::copy(parts, at).out), ...);
      return out;
    }

    constexpr auto Rows = Joined(Drawing, Dimensions, Drafting, Axes, Space, Styles, Scene);
  } // namespace

  auto All() -> std::span<const Row>
  {
    return Rows;
  }
} // namespace DxfIO::Header::Rows
