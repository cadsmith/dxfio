#ifndef DXFIO_CREATE_HPP
#define DXFIO_CREATE_HPP

#include "dxfio/api.hpp"
#include "dxfio/document.hpp"
#include "dxfio/version.hpp"

namespace DxfIO
{
  /**
   *  An empty drawing of that release, which AutoCAD opens without a finding: layer 0, the standard
   *  linetypes, text and dimension styles, model and paper space, and the objects the release
   *  needs.
   */
  [[nodiscard]] DXFIO_API auto Create(Version release = Version::R2018) -> Document;
} // namespace DxfIO

#endif /* DXFIO_CREATE_HPP */
