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
  /**
   *  Hashes a name by its letters in upper case, so names that differ only in case hash alike.
   */
  struct Folded
  {
    // NOLINTNEXTLINE(readability-identifier-naming): the name the standard looks for
    using is_transparent = void;

    auto operator()(std::string_view name) const noexcept -> std::size_t;
  };

  /**
   *  Compares names as AutoCAD does: without case.
   */
  struct Alike
  {
    // NOLINTNEXTLINE(readability-identifier-naming): the name the standard looks for
    using is_transparent = void;

    auto operator()(std::string_view one, std::string_view other) const noexcept -> bool;
  };

  /**
   *  Names found as AutoCAD finds them, without case, in a time that does not grow with their
   *  number.
   */
  using Names = std::unordered_set<std::string, Folded, Alike>;

  /**
   *  The names of a table's records; a record with no name, which nothing may name, gives none.
   */
  template <class TableT>
  auto Named(const TableT &table) -> Names
  {
    Names out;
    out.reserve(table.Records.size());
    for (const auto &one : table.Records)
    {
      if (!one.Record.Name.empty())
      {
        out.emplace(one.Record.Name);
      }
    }
    return out;
  }

  /**
   *  The names of the blocks; one with no name gives none.
   */
  auto Defined(std::span<const Blocks::Block> blocks) -> Names;
} // namespace DxfIO::Audit

#endif /* DXFIO_AUDIT_NAMES_HPP */
