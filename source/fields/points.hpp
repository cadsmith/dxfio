#ifndef DXFIO_FIELDS_POINTS_HPP
#define DXFIO_FIELDS_POINTS_HPP

#include "fields/context.hpp"
#include "fields/value.hpp"
#include "fields/walk.hpp"

#include "dxfio/failure.hpp"
#include "dxfio/vector.hpp"

#include <expected>
#include <optional>

namespace DxfIO::Fields
{
inline constexpr int Axis = 10;

template <>
struct Value<Vector>
{
  static auto Read(Walk &in, int code) -> std::optional<Vector>;
  static auto Write(Writing &to, int code, const Vector &held) -> std::expected<void, Failure>;
};
} // namespace DxfIO::Fields

#endif /* DXFIO_FIELDS_POINTS_HPP */
