#ifndef DXFIO_FIELDS_NUMBERS_HPP
#define DXFIO_FIELDS_NUMBERS_HPP

#include "fields/context.hpp"
#include "fields/value.hpp"
#include "fields/walk.hpp"

#include "dxfio/failure.hpp"

#include <cstdint>
#include <expected>
#include <optional>

namespace DxfIO::Fields
{
  template <>
  struct Value<int>
  {
    static auto Read(Walk &in, int code) -> std::optional<int>;
    static auto Write(Writing &to, int code, int held) -> std::expected<void, Failure>;
  };

  template <>
  struct Value<std::int64_t>
  {
    static auto Read(Walk &in, int code) -> std::optional<std::int64_t>;
    static auto Write(Writing &to, int code, std::int64_t held) -> std::expected<void, Failure>;
  };

  template <>
  struct Value<double>
  {
    static auto Read(Walk &in, int code) -> std::optional<double>;
    static auto Write(Writing &to, int code, double held) -> std::expected<void, Failure>;
  };

  template <>
  struct Value<bool>
  {
    static auto Read(Walk &in, int code) -> std::optional<bool>;
    static auto Write(Writing &to, int code, bool held) -> std::expected<void, Failure>;
  };
} // namespace DxfIO::Fields

#endif /* DXFIO_FIELDS_NUMBERS_HPP */
