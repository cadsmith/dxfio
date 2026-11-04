#ifndef DXFIO_PLAN_FOREIGN_HPP
#define DXFIO_PLAN_FOREIGN_HPP

#include "dxfio/api.hpp"
#include "dxfio/object.hpp"

#include <vector>

namespace DxfIO::Plan
{
[[nodiscard]] DXFIO_API auto Foreign(std::vector<Object> all) -> std::vector<Object>;
} // namespace DxfIO::Plan

#endif /* DXFIO_PLAN_FOREIGN_HPP */
