#ifndef DXFIO_FILE_SECTION_HPP
#define DXFIO_FILE_SECTION_HPP

#include "file/record.hpp"
#include "tags/tag.hpp"

#include "dxfio/api.hpp"

#include <span>
#include <string_view>
#include <vector>

namespace DxfIO::File
{
struct DXFIO_API Variable
{
  std::string_view Name;
  std::span<const Tags::Tag> Values;
};

struct DXFIO_API Nest
{
  Record Head;
  std::vector<Record> Records;
  Record End;
};

struct DXFIO_API Section
{
  std::string_view Name;
  std::span<const Tags::Tag> Body;
};
} // namespace DxfIO::File

#endif /* DXFIO_FILE_SECTION_HPP */
