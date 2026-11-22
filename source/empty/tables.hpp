#ifndef DXFIO_EMPTY_TABLES_HPP
#define DXFIO_EMPTY_TABLES_HPP

#include "roots.hpp"
#include "seed.hpp"

#include "dxfio/drawing.hpp"

namespace DxfIO::Empty
{
  void Tables(Drawing &drawing, const Roots &ids, Seed &handles);
} // namespace DxfIO::Empty

#endif /* DXFIO_EMPTY_TABLES_HPP */
