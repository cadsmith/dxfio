#ifndef DXFIO_HEADER_ROW_HPP
#define DXFIO_HEADER_ROW_HPP

#include "dxfio/api.hpp"
#include "dxfio/handle.hpp"
#include "dxfio/vector.hpp"
#include "dxfio/version.hpp"

#include <cstdint>
#include <string_view>
#include <variant>

namespace DxfIO::Header
{
  using Value = std::variant<std::int64_t, double, bool, std::string_view, Handle, Vector>;

  struct DXFIO_API Row
  {
    std::string_view Name;
    int              Code{};
    Header::Value    Value;
    bool             Flat{};
    Version          Since{Version::R10};
    Version          Until{Version::R2018};
  };
} // namespace DxfIO::Header

#endif /* DXFIO_HEADER_ROW_HPP */
