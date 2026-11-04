#include "tags/tag.hpp"

namespace DxfIO::Tags
{
auto Tag::operator==(const Tag &other) const -> bool
{
  return Code == other.Code && Value == other.Value;
}

auto Text(const Tag &one) -> std::string_view
{
  const auto *held = std::get_if<std::string_view>(&one.Value);
  return held == nullptr ? std::string_view{} : *held;
}

auto Holds(const Tag &one, int code, std::string_view value) -> bool
{
  return one.Code == code && std::holds_alternative<std::string_view>(one.Value) && Text(one) == value;
}
} // namespace DxfIO::Tags
