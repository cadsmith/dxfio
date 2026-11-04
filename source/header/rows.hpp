#ifndef DXFIO_HEADER_ROWS_HPP
#define DXFIO_HEADER_ROWS_HPP

#include "header/row.hpp"

#include "dxfio/api.hpp"

#include <span>

namespace DxfIO::Header::Rows
{
[[nodiscard]] DXFIO_API auto All() -> std::span<const Row>;
} // namespace DxfIO::Header::Rows

#endif /* DXFIO_HEADER_ROWS_HPP */
