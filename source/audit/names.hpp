#ifndef DXFIO_AUDIT_NAMES_HPP
#define DXFIO_AUDIT_NAMES_HPP

#include "dxfio/blocks/blocks.hpp"

#include <cstddef>
#include <span>
#include <string>
#include <string_view>
#include <unordered_set>

namespace DxfIO::Audit
{
// Hashes a name by its letters in upper case, so names that differ only in case hash alike.
struct Folded
{
  using is_transparent = void; // NOLINT(readability-identifier-naming): the name the standard looks for

  auto operator()(std::string_view name) const noexcept -> std::size_t;
};

// Compares names as AutoCAD does: without case.
struct Alike
{
  using is_transparent = void; // NOLINT(readability-identifier-naming): the name the standard looks for

  auto operator()(std::string_view one, std::string_view other) const noexcept -> bool;
};

// Names found as AutoCAD finds them, without case, in a time that does not grow with their number.
using Names = std::unordered_set<std::string, Folded, Alike>;

// The names of a table's records.
template <class TableT>
auto Named(const TableT &table) -> Names
{
  Names out;
  out.reserve(table.Records.size());
  for (const auto &one : table.Records)
  {
    out.emplace(one.Record.Name);
  }
  return out;
}

// The names of the blocks.
auto Defined(std::span<const Blocks::Block> blocks) -> Names;
} // namespace DxfIO::Audit

#endif /* DXFIO_AUDIT_NAMES_HPP */
