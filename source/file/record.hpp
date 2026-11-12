#ifndef DXFIO_FILE_RECORD_HPP
#define DXFIO_FILE_RECORD_HPP

#include "tags/tag.hpp"

#include "dxfio/api.hpp"

#include <span>
#include <string_view>

namespace DxfIO::File
{
  struct DXFIO_API Record
  {
    std::string_view           Name;
    std::span<const Tags::Tag> Tags;
  };
} // namespace DxfIO::File

#endif /* DXFIO_FILE_RECORD_HPP */
