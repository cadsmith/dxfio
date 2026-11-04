#include "dxfio/tables/face.hpp"

#include "dxfio/group.hpp"
#include "dxfio/xdata.hpp"

#include <algorithm>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace DxfIO::Tables
{
namespace
{
constexpr std::string_view Owner = "ACAD";

constexpr int Family = 1000;

constexpr int Flags = 1071;

template <class ValueT>
auto Held(const Xdata &all, int code) -> const ValueT *
{
  const auto found = std::ranges::find(all.Groups, code, &Group::Code);
  return found == all.Groups.end() ? nullptr : std::get_if<ValueT>(&found->Value);
}
} // namespace

auto Named(const Style &one) -> std::optional<Face>
{
  const std::vector<Xdata> &all = one.Record.Base.Extended;
  const auto found = std::ranges::find(all, Owner, &Xdata::Application);
  if (found == all.end())
  {
    return std::nullopt;
  }
  const auto *const name = Held<std::string>(*found, Family);
  if (name == nullptr)
  {
    return std::nullopt;
  }
  const auto *const bits = Held<std::int64_t>(*found, Flags);
  return Face{.Family = *name, .Flags = bits != nullptr ? *bits : 0};
}
} // namespace DxfIO::Tables
