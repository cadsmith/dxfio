#ifndef DXFIO_EDIT_SEEDING_HPP
#define DXFIO_EDIT_SEEDING_HPP

#include "seed.hpp"

#include "dxfio/document.hpp"
#include "dxfio/failure.hpp"
#include "dxfio/handle.hpp"

#include <expected>

namespace DxfIO::Edit
{
// The handles an edit takes from: $HANDSEED, or past every handle the drawing holds when it has none.
[[nodiscard]] auto Seeded(const Document &drawing) -> std::expected<Seed, Failure>;

// Moves $HANDSEED to the handle after the last one an edit took, and adds it to the header when it has none, so the
// next edit takes from it rather than looking past every handle again.
void Moved(Document &drawing, Handle next);
} // namespace DxfIO::Edit

#endif /* DXFIO_EDIT_SEEDING_HPP */
