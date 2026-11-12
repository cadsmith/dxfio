#ifndef DXFIO_FIELDS_MATRICES_HPP
#define DXFIO_FIELDS_MATRICES_HPP

#include "fields/context.hpp"
#include "fields/value.hpp"
#include "fields/walk.hpp"

#include "dxfio/failure.hpp"
#include "dxfio/matrix.hpp"

#include <expected>
#include <optional>

namespace DxfIO::Fields
{
  template <>
  struct Value<Matrix>
  {
    static auto Read(Walk &in, int code) -> std::optional<Matrix>;
    static auto Write(Writing &to, int code, const Matrix &held) -> std::expected<void, Failure>;
  };
} // namespace DxfIO::Fields

#endif /* DXFIO_FIELDS_MATRICES_HPP */
