#ifndef DXFIO_FILE_SECTIONS_NESTS_HPP
#define DXFIO_FILE_SECTIONS_NESTS_HPP

#include "file/section.hpp"
#include "tags/tag.hpp"

#include "dxfio/failure.hpp"
#include "dxfio/issue.hpp"

#include <expected>
#include <span>
#include <vector>

namespace DxfIO::File::Sections
{
  /**
   *  The tables of a TABLES section. A table missing its ENDTAB closes at the next TABLE or at the
   *  end of the section, told to issues, when given.
   */
  auto Tables(std::span<const Tags::Tag> body, std::vector<Issue> *issues = nullptr)
      -> std::expected<std::vector<Nest>, Failure>;

  /**
   *  The blocks of a BLOCKS section. A block missing its ENDBLK closes at the next BLOCK or at the
   *  end of the section, told to issues, when given.
   */
  auto Blocks(std::span<const Tags::Tag> body, std::vector<Issue> *issues = nullptr)
      -> std::expected<std::vector<Nest>, Failure>;
} // namespace DxfIO::File::Sections

#endif /* DXFIO_FILE_SECTIONS_NESTS_HPP */
