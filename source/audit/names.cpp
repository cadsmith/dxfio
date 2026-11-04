#include "names.hpp"

#include "ascii.hpp"

#include <cstdint>

namespace DxfIO::Audit
{
namespace
{
constexpr std::uint64_t Basis = 14695981039346656037ULL;
constexpr std::uint64_t Prime = 1099511628211ULL;
} // namespace

auto Folded::operator()(std::string_view name) const noexcept -> std::size_t
{
  std::uint64_t out = Basis;
  for (const char letter : name)
  {
    out = (out ^ static_cast<unsigned char>(Ascii::Upper(letter))) * Prime;
  }
  return static_cast<std::size_t>(out);
}

auto Alike::operator()(std::string_view one, std::string_view other) const noexcept -> bool
{
  return Ascii::Same(one, other);
}

auto Defined(std::span<const Blocks::Block> blocks) -> Names
{
  Names out;
  out.reserve(blocks.size());
  for (const Blocks::Block &one : blocks)
  {
    out.emplace(one.Begin.Name);
  }
  return out;
}
} // namespace DxfIO::Audit
