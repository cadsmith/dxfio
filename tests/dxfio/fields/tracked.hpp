#ifndef DXFIO_TESTS_DXF_FIELDS_TRACKED_HPP
#define DXFIO_TESTS_DXF_FIELDS_TRACKED_HPP

#include "fields/value.hpp"
#include "fields/walk.hpp"

#include <optional>
#include <utility>

namespace DxfIO::Fields
{
  struct Tracked
  {
    Tracked() = default;

    Tracked(const Tracked & /*other*/) : Copied(true) {}

    Tracked(Tracked &&) noexcept = default;

    auto operator=(const Tracked & /*other*/) -> Tracked &
    {
      Copied = true;
      return *this;
    }

    auto operator=(Tracked &&) noexcept -> Tracked & = default;
    ~Tracked()                                       = default;

    bool Copied{};
  };

  template <>
  struct Value<Tracked>
  {
    static auto Read(Walk &in, int code) -> std::optional<Tracked>
    {
      return in.Take(code) == nullptr ? std::nullopt : std::optional<Tracked>{std::in_place};
    }
  };
} // namespace DxfIO::Fields

#endif /* DXFIO_TESTS_DXF_FIELDS_TRACKED_HPP */
