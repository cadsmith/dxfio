#ifndef DXFIO_DOWN_DOWN_HPP
#define DXFIO_DOWN_DOWN_HPP

#include "dxfio/api.hpp"
#include "dxfio/drawing.hpp"
#include "dxfio/issue.hpp"
#include "dxfio/version.hpp"

#include <vector>

namespace DxfIO::Down
{
  [[nodiscard]] DXFIO_API auto Narrowed(Drawing from, Version release, std::vector<Issue> *issues)
      -> Drawing;
} // namespace DxfIO::Down

#endif /* DXFIO_DOWN_DOWN_HPP */
