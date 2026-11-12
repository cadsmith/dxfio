#ifndef DXFIO_FIELDS_OPTIONAL_HPP
#define DXFIO_FIELDS_OPTIONAL_HPP

#include "fields/context.hpp"
#include "fields/value.hpp"
#include "fields/walk.hpp"

#include "dxfio/failure.hpp"

#include <expected>
#include <optional>
#include <utility>

namespace DxfIO::Fields
{
  template <class ValueT>
  struct Value<std::optional<ValueT>>
  {
    static auto Read(Walk &in, int code) -> std::optional<std::optional<ValueT>>
    {
      return Value<ValueT>::Read(in, code).transform(
          [](ValueT &&held) -> auto { return std::optional<ValueT>{std::move(held)}; });
    }

    static auto Write(Writing &to, int code, const std::optional<ValueT> &held)
        -> std::expected<void, Failure>
    {
      return held ? Value<ValueT>::Write(to, code, *held) : std::expected<void, Failure>{};
    }
  };
} // namespace DxfIO::Fields

#endif /* DXFIO_FIELDS_OPTIONAL_HPP */
