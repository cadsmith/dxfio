#ifndef DXFIO_EMPTY_TABLES_HPP
#define DXFIO_EMPTY_TABLES_HPP

#include "roots.hpp"
#include "seed.hpp"

#include "dxfio/document.hpp"

namespace DxfIO::Empty
{
  void Tables(Document &drawing, const Roots &ids, Seed &handles);
} // namespace DxfIO::Empty

#endif /* DXFIO_EMPTY_TABLES_HPP */
