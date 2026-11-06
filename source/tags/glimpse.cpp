#include "tags/glimpse.hpp"

#include <cstddef>
#include <format>

namespace DxfIO::Tags
{
namespace
{
constexpr std::size_t Echoed = 64;
} // namespace

auto Glimpse(std::string_view text) -> std::string
{
  return text.size() > Echoed ? std::format("{}...", text.substr(0, Echoed)) : std::string{text};
}
} // namespace DxfIO::Tags
