#ifndef DXFIO_FIELDS_HANDLES_HPP
#define DXFIO_FIELDS_HANDLES_HPP

#include "fields/context.hpp"
#include "fields/value.hpp"
#include "fields/walk.hpp"

#include "dxfio/failure.hpp"
#include "dxfio/handle.hpp"

#include <expected>
#include <optional>

namespace DxfIO::Fields
{
  template <>
  struct Value<Handle>
  {
    static auto Read(Walk &in, int code) -> std::optional<Handle>;
    static auto Write(Writing &to, int code, Handle held) -> std::expected<void, Failure>;
  };
} // namespace DxfIO::Fields

#endif /* DXFIO_FIELDS_HANDLES_HPP */
