#ifndef DXFIO_EMPTY_WHOLE_HPP
#define DXFIO_EMPTY_WHOLE_HPP

#include "seed.hpp"

#include "dxfio/document.hpp"
#include "dxfio/version.hpp"

namespace DxfIO::Empty
{
  /**
   *  An empty drawing of a release from R13 on, its handles taken from the seed: the tables,
   *  blocks, layouts and objects the release needs, its header, and $HANDSEED past what was taken.
   */
  [[nodiscard]] auto Whole(Version release, Seed &handles) -> Document;
} // namespace DxfIO::Empty

#endif /* DXFIO_EMPTY_WHOLE_HPP */
