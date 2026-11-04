#ifndef DXFIO_DOWN_DOWN_HPP
#define DXFIO_DOWN_DOWN_HPP

#include "dxfio/api.hpp"
#include "dxfio/document.hpp"
#include "dxfio/issue.hpp"
#include "dxfio/version.hpp"

#include <vector>

namespace DxfIO::Down
{
[[nodiscard]] DXFIO_API auto Narrowed(Document from, Version release, std::vector<Issue> *issues) -> Document;
} // namespace DxfIO::Down

#endif /* DXFIO_DOWN_DOWN_HPP */
