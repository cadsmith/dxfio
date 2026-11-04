#ifndef DXFIO_AUDIT_WALK_HPP
#define DXFIO_AUDIT_WALK_HPP

#include "ascii.hpp"
#include "entities/io.hpp"

#include "dxfio/document.hpp"

#include <algorithm>
#include <string_view>

namespace DxfIO::Audit
{
template <class DocumentT, class VisitT>
void Lists(DocumentT &drawing, const VisitT &visit)
{
  visit(drawing.Entities);
  for (auto &one : drawing.Blocks)
  {
    visit(one.Entities);
  }
}

template <class RecordT>
auto Holds(const Tables::Table<RecordT> &table, std::string_view name) -> bool
{
  return std::ranges::any_of(table.Records,
                             [name](const RecordT &one) -> bool { return DxfIO::Ascii::Same(one.Record.Name, name); });
}
} // namespace DxfIO::Audit

#endif /* DXFIO_AUDIT_WALK_HPP */
