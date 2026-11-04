#ifndef DXFIO_TABLES_SHAPES_RECORD_HPP
#define DXFIO_TABLES_SHAPES_RECORD_HPP

#include "fields/field.hpp"
#include "fields/numbers.hpp"
#include "fields/subclass.hpp"
#include "fields/text.hpp"
#include "shapes.hpp"

#include "dxfio/tables/record.hpp"

#include <tuple>

namespace DxfIO
{
template <>
inline constexpr auto Shapes<Tables::Record> = std::tuple{
    Fields::Field{.Code = 2, .Member = &Tables::Record::Name},
    Fields::Field{.Code = 70, .Member = &Tables::Record::Flags},
};
} // namespace DxfIO

namespace DxfIO::Tables
{
inline constexpr Fields::Subclass Opening{.Marker = "AcDbSymbolTableRecord"};
} // namespace DxfIO::Tables

#endif /* DXFIO_TABLES_SHAPES_RECORD_HPP */
