#ifndef DXFIO_TABLES_APPID_HPP
#define DXFIO_TABLES_APPID_HPP

#include "dxfio/api.hpp"
#include "dxfio/tables/record.hpp"

namespace DxfIO::Tables
{
  /**
   *  An APPID table record: an application registered to attach extended data, named after it.
   */
  struct DXFIO_API Appid
  {
    /**
     *  The name, flags and handles every table record carries.
     */
    Tables::Record Record{};

    auto operator==(const Appid &) const -> bool = default;
  };
} // namespace DxfIO::Tables

#endif /* DXFIO_TABLES_APPID_HPP */
