#ifndef DXFIO_HEADER_CREATED_HPP
#define DXFIO_HEADER_CREATED_HPP

#include "dxfio/api.hpp"
#include "dxfio/header/header.hpp"
#include "dxfio/version.hpp"

#include <vector>

namespace DxfIO::Header
{
  [[nodiscard]] DXFIO_API auto Created(Version release = Version::R2018) -> std::vector<Variable>;
} // namespace DxfIO::Header

#endif /* DXFIO_HEADER_CREATED_HPP */
