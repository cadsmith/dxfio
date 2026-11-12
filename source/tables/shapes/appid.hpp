#ifndef DXFIO_TABLES_SHAPES_APPID_HPP
#define DXFIO_TABLES_SHAPES_APPID_HPP

#include "fields/part.hpp"
#include "record.hpp"

#include "dxfio/tables/appid.hpp"

#include <string_view>

namespace DxfIO
{
  template <>
  inline constexpr auto Shapes<Tables::Appid> = std::tuple{
      Tables::Opening,
      Fields::Subclass{.Marker = "AcDbRegAppTableRecord"},
      Fields::Part{
          .Member = &Tables::Appid::Record,
          .Fields = Shapes<Tables::Record>,
      },
  };

  template <>
  inline constexpr std::string_view Type<Tables::Appid> = "APPID";
} // namespace DxfIO

#endif /* DXFIO_TABLES_SHAPES_APPID_HPP */
