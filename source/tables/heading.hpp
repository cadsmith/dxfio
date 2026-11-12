#ifndef DXFIO_TABLES_HEADING_HPP
#define DXFIO_TABLES_HEADING_HPP

#include "fields/context.hpp"
#include "file/record.hpp"

#include "dxfio/failure.hpp"
#include "dxfio/issue.hpp"
#include "dxfio/tables/head.hpp"

#include <cstddef>
#include <expected>
#include <vector>

namespace DxfIO::Tables
{
  auto Headed(const File::Record &one, const Fields::Reading &with) -> Tables::Head;

  auto Headed(Fields::Writing &to, const Tables::Head &one) -> std::expected<void, Failure>;
} // namespace DxfIO::Tables

#endif /* DXFIO_TABLES_HEADING_HPP */
