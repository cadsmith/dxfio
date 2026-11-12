#ifndef DXFIO_OBJECTS_DATATABLE_HPP
#define DXFIO_OBJECTS_DATATABLE_HPP

#include "dxfio/api.hpp"
#include "dxfio/base.hpp"
#include "dxfio/group.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace DxfIO::Objects
{
  /**
   *  One column of a data table, its cells kept as the groups they came in.
   */
  struct DXFIO_API Column
  {
    /**
     *  The type of the column's values, such as 1 for integers or 2 for reals. Group 92.
     */
    int Kind{};
    /**
     *  The name of the column. Group 2.
     */
    std::string Name{};
    /**
     *  The cells of the column, top to bottom, as groups whose code tells the type of each value.
     */
    std::vector<DxfIO::Group> Cells{};

    auto operator==(const Column &) const -> bool = default;
  };

  /**
   *  Which name a data table goes by: it is ACDBDATATABLE in R2000 and DATATABLE after it.
   */
  enum class Edition : std::uint8_t
  {
    /**
     *  ACDBDATATABLE, as R2000 writes it.
     */
    Early,
    /**
     *  DATATABLE, as R2004 and later write it.
     */
    Current,
  };

  /**
   *  A data table: values in named columns, row by row, written as ACDBDATATABLE or DATATABLE by
   *  release.
   */
  template <Edition KindV>
  struct DataTableOf
  {
    /**
     *  The handle, owner, reactors and extended data.
     */
    DxfIO::Base Base{};
    /**
     *  The version of the object, 2 as AutoCAD writes it. Group 70.
     */
    int Version{2};
    /**
     *  The number of rows. Group 91.
     */
    int Rows{};
    /**
     *  The name of the table. Group 1.
     */
    std::string Name{};
    /**
     *  The columns, left to right; their count is group 90.
     */
    std::vector<Column> Columns{};

    auto operator==(const DataTableOf &) const -> bool = default;
  };

  /**
   *  A DATATABLE, the name from R2004.
   */
  using DataTable = DataTableOf<Edition::Current>;
  /**
   *  An ACDBDATATABLE, the name in R2000.
   */
  using AcDbDataTable = DataTableOf<Edition::Early>;
} // namespace DxfIO::Objects

#endif /* DXFIO_OBJECTS_DATATABLE_HPP */
