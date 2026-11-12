#ifndef DXFIO_TABLES_TABLE_HPP
#define DXFIO_TABLES_TABLE_HPP

#include "dxfio/tables/head.hpp"

#include <vector>

namespace DxfIO::Tables
{
  /**
   *  A table of the TABLES section: its TABLE head and the records of one kind it holds.
   */
  template <class RecordT>
  struct Table
  {
    /**
     *  The TABLE record that opens the table.
     */
    Tables::Head Head{};
    /**
     *  The records of the table, in the order the file gives them.
     */
    std::vector<RecordT> Records{};

    auto operator==(const Table &) const -> bool = default;
  };
} // namespace DxfIO::Tables

#endif /* DXFIO_TABLES_TABLE_HPP */
