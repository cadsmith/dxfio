#ifndef DXFIO_EMPTY_CLASSES_HPP
#define DXFIO_EMPTY_CLASSES_HPP

#include "raw/io.hpp"

#include "dxfio/object.hpp"
#include "dxfio/raw.hpp"
#include "dxfio/version.hpp"

#include <span>
#include <vector>

namespace DxfIO::Empty
{
auto Classes(Version release, std::span<const Object> all) -> std::vector<Raw>;
} // namespace DxfIO::Empty

#endif /* DXFIO_EMPTY_CLASSES_HPP */
