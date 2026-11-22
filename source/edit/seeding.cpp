#include "seeding.hpp"

#include "handseed.hpp"
#include "past.hpp"

#include <algorithm>

namespace DxfIO::Edit
{
  auto Seeded(const Drawing &drawing) -> std::expected<Seed, Failure>
  {
    const Handle first = Past(drawing);
    if (!first)
    {
      return std::unexpected{
          Failure{.Code = FailureCode::Refused, .What = "no handle left past the last one"}};
    }
    return Seed{first};
  }

  void Moved(Drawing &drawing, Handle next)
  {
    WriteSeed(drawing, std::max(next, ReadSeed(drawing)));
  }
} // namespace DxfIO::Edit
