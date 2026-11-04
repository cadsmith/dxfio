#ifndef DXFIO_EMPTY_OBJECTS_HPP
#define DXFIO_EMPTY_OBJECTS_HPP

#include "roots.hpp"
#include "seed.hpp"

#include "dxfio/handle.hpp"
#include "dxfio/object.hpp"

#include <vector>

namespace DxfIO::Empty
{
auto Objects(const Roots &ids, Handle viewport, Seed &handles) -> std::vector<Object>;

auto Early(const Roots &ids, Seed &handles) -> std::vector<Object>;
} // namespace DxfIO::Empty

#endif /* DXFIO_EMPTY_OBJECTS_HPP */
