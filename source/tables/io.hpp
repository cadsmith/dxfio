#ifndef DXFIO_TABLES_IO_HPP
#define DXFIO_TABLES_IO_HPP

#include "fields/context.hpp"
#include "file/section.hpp"

#include "dxfio/api.hpp"
#include "dxfio/document.hpp"
#include "dxfio/failure.hpp"
#include "dxfio/issue.hpp"

#include <expected>
#include <span>
#include <vector>

namespace DxfIO::Tables
{
  DXFIO_API void               Read(std::span<const File::Nest> all, Document &drawing,
                                    const Fields::Reading &with);
  [[nodiscard]] DXFIO_API auto Write(Fields::Writing &to, const Document &drawing)
      -> std::expected<void, Failure>;
} // namespace DxfIO::Tables

#endif /* DXFIO_TABLES_IO_HPP */
