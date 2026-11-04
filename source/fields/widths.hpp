#ifndef DXFIO_FIELDS_WIDTHS_HPP
#define DXFIO_FIELDS_WIDTHS_HPP

#include "fields/context.hpp"
#include "fields/value.hpp"
#include "fields/walk.hpp"

#include "dxfio/failure.hpp"
#include "dxfio/widths.hpp"

#include <expected>
#include <optional>

namespace DxfIO::Fields
{
template <>
struct Value<Widths>
{
  static auto Read(Walk &in, int code) -> std::optional<Widths>;
  static auto Write(Writing &to, int code, const Widths &held) -> std::expected<void, Failure>;
};
} // namespace DxfIO::Fields

#endif /* DXFIO_FIELDS_WIDTHS_HPP */
