#ifndef DXFIO_TAGS_REAL_HPP
#define DXFIO_TAGS_REAL_HPP

#include "dxfio/api.hpp"

#include <optional>
#include <string>

namespace DxfIO::Tags
{
[[nodiscard]] DXFIO_API auto Real(double value) -> std::optional<std::string>;
} // namespace DxfIO::Tags

#endif /* DXFIO_TAGS_REAL_HPP */
