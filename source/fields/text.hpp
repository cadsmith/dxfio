#ifndef DXFIO_FIELDS_TEXT_HPP
#define DXFIO_FIELDS_TEXT_HPP

#include "fields/context.hpp"
#include "fields/value.hpp"
#include "fields/walk.hpp"

#include "dxfio/api.hpp"
#include "dxfio/failure.hpp"
#include "dxfio/symbol.hpp"

#include <expected>
#include <optional>
#include <string>
#include <string_view>

namespace DxfIO::Fields
{
  template <>
  struct Value<std::string>
  {
    static auto Read(Walk &in, int code) -> std::optional<std::string>;
    static auto Write(Writing &to, int code, std::string_view held) -> std::expected<void, Failure>;
  };

  [[nodiscard]] DXFIO_API auto Word(Writing &to, int code, std::string_view text)
      -> std::expected<void, Failure>;

  template <>
  struct Value<Symbol>
  {
    static auto Read(Walk &in, int code) -> std::optional<Symbol>;
    static auto Write(Writing &to, int code, const Symbol &held) -> std::expected<void, Failure>;
  };
} // namespace DxfIO::Fields

#endif /* DXFIO_FIELDS_TEXT_HPP */
