#ifndef DXFIO_FILE_FILE_HPP
#define DXFIO_FILE_FILE_HPP

#include "file/section.hpp"
#include "tags/tag.hpp"

#include "dxfio/api.hpp"
#include "dxfio/failure.hpp"
#include "dxfio/issue.hpp"

#include <expected>
#include <span>
#include <vector>

namespace DxfIO::File
{
  /**
   *  The sections of a file, each its groups between its name and its ENDSEC, up to EOF. A last
   *  section with no ENDSEC is closed at the end of the file, and a TABLE outside a TABLES section
   *  opens one; each is told to issues, when given.
   */
  [[nodiscard]] DXFIO_API auto Read(std::span<const Tags::Tag> all,
                                    std::vector<Issue>        *issues = nullptr)
      -> std::expected<std::vector<Section>, Failure>;
} // namespace DxfIO::File

#endif /* DXFIO_FILE_FILE_HPP */
