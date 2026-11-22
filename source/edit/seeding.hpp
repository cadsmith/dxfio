#ifndef DXFIO_EDIT_SEEDING_HPP
#define DXFIO_EDIT_SEEDING_HPP

#include "seed.hpp"

#include "dxfio/drawing.hpp"
#include "dxfio/failure.hpp"
#include "dxfio/handle.hpp"

#include <expected>

namespace DxfIO::Edit
{
  /**
   *  The handles an edit takes from: past every handle the drawing holds and its $HANDSEED, so a
   *  seed left behind the handles in use hands out none of them.
   */
  [[nodiscard]] auto Seeded(const Drawing &drawing) -> std::expected<Seed, Failure>;

  /**
   *  Moves $HANDSEED to the handle after the last one an edit took, and adds it to the header when
   *  it has none, so the next edit takes from it rather than looking past every handle again.
   */
  void Moved(Drawing &drawing, Handle next);
} // namespace DxfIO::Edit

#endif /* DXFIO_EDIT_SEEDING_HPP */
