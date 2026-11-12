#include "past.hpp"

#include "handseed.hpp"
#include "held.hpp"

#include <algorithm>
#include <cstdint>
#include <limits>

namespace DxfIO
{
  namespace
  {
    constexpr Handle Last{.Value = std::numeric_limits<std::uint64_t>::max()};
  } // namespace

  auto Past(const Document &from) -> Handle
  {
    Handle held{};
    Held(from, [&held](Handle one) -> void { held = std::max(held, one); });
    const Handle out = std::max(Handle{.Value = held.Value + 1}, ReadSeed(from));
    return held == Last || out == Last ? Handle{} : out;
  }
} // namespace DxfIO
