#include "seeding.hpp"

#include "handseed.hpp"
#include "past.hpp"

#include <algorithm>
#include <cstdint>
#include <limits>

namespace DxfIO::Edit
{
namespace
{
constexpr std::uint64_t Last = std::numeric_limits<std::uint64_t>::max();
} // namespace

auto Seeded(const Document &drawing) -> std::expected<Seed, Failure>
{
  const Handle held = ReadSeed(drawing);
  const Handle first = held ? held : Past(drawing);
  if (!first || first.Value == Last)
  {
    return std::unexpected{Failure{.Line = 0, .What = "the drawing holds the last handle there is"}};
  }
  return Seed{first};
}

void Moved(Document &drawing, Handle next)
{
  WriteSeed(drawing, std::max(next, ReadSeed(drawing)));
}
} // namespace DxfIO::Edit
