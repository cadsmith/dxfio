#ifndef DXFIO_FIELDS_CORNERS_HPP
#define DXFIO_FIELDS_CORNERS_HPP

#include "fields/context.hpp"
#include "fields/value.hpp"
#include "fields/walk.hpp"

#include "dxfio/corners.hpp"
#include "dxfio/failure.hpp"

#include <expected>
#include <optional>

namespace DxfIO::Fields
{
  template <>
  struct Value<Corners>
  {
    static auto Read(Walk &in, int code) -> std::optional<Corners>;
    static auto Write(Writing &to, int code, const Corners &held) -> std::expected<void, Failure>;
  };
} // namespace DxfIO::Fields

#endif /* DXFIO_FIELDS_CORNERS_HPP */
