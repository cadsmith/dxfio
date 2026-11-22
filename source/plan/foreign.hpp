#ifndef DXFIO_PLAN_FOREIGN_HPP
#define DXFIO_PLAN_FOREIGN_HPP

#include "dxfio/api.hpp"
#include "dxfio/drawing.hpp"
#include "dxfio/issue.hpp"

#include <vector>

namespace DxfIO::Plan
{
  /**
   *  Leaves out the objects of a drawing saved to a later release whose kind dxfio does not type,
   *  as their groups are in the form of the release they were read from, each told to issues when
   *  given; the entries, defaults, reactors and extension dictionaries that named them go too.
   */
  DXFIO_API void Foreign(Drawing &drawing, std::vector<Issue> *issues = nullptr);
} // namespace DxfIO::Plan

#endif /* DXFIO_PLAN_FOREIGN_HPP */
