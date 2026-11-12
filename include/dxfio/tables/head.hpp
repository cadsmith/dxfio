#ifndef DXFIO_TABLES_HEAD_HPP
#define DXFIO_TABLES_HEAD_HPP

#include "dxfio/api.hpp"
#include "dxfio/base.hpp"

#include <string>

namespace DxfIO::Tables
{
  /**
   *  The TABLE record that opens a table of the TABLES section.
   */
  struct DXFIO_API Head
  {
    /**
     *  The handle, owner, reactors and extended data.
     */
    DxfIO::Base Base{};
    /**
     *  The name of the table, such as "LAYER". Group 2.
     */
    std::string Name{};
    /**
     *  The most entries the table holds, as the file gives it. Group 70.
     */
    int Count{};

    auto operator==(const Head &) const -> bool = default;
  };
} // namespace DxfIO::Tables

#endif /* DXFIO_TABLES_HEAD_HPP */
