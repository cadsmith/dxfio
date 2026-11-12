#ifndef DXFIO_TABLES_RECORD_HPP
#define DXFIO_TABLES_RECORD_HPP

#include "dxfio/api.hpp"
#include "dxfio/base.hpp"

#include <string>

namespace DxfIO::Tables
{
  /**
   *  What every table record carries in its AcDbSymbolTableRecord subclass: its handles, name and
   *  flags.
   */
  struct DXFIO_API Record
  {
    /**
     *  The handle, owner, reactors and extended data.
     */
    DxfIO::Base Base{};
    /**
     *  The name entities and other records know the record by. Group 2.
     */
    std::string Name{};
    /**
     *  Bits: 16 when the record comes from an xref, 32 once that xref is resolved, 64 when an
     *  entity used it the last time the drawing was edited; the low bits are the table's own, such
     *  as a frozen layer's 1. Group 70.
     */
    int Flags{};

    auto operator==(const Record &) const -> bool = default;
  };
} // namespace DxfIO::Tables

#endif /* DXFIO_TABLES_RECORD_HPP */
