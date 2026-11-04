#ifndef DXFIO_AUDIT_WALK_HPP
#define DXFIO_AUDIT_WALK_HPP

#include "../based.hpp"
#include "../objects/shapes/defaulted.hpp"
#include "ascii.hpp"
#include "entities/io.hpp"

#include "dxfio/document.hpp"

#include <algorithm>
#include <string_view>
#include <variant>

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

template <class ChainT, class VisitT>
void Chained(ChainT &chain, const VisitT &visit)
{
  for (auto &child : chain.Children)
  {
    visit(child.Record.Base);
  }
  if (chain.Closing)
  {
    visit((**chain.Closing).Record.Base);
  }
}

// Calls visit with the Base of an entity, then those of the vertices or attributes it carries and of their SEQEND.
template <class EntityT, class VisitT>
void Carried(EntityT &one, const VisitT &visit)
{
  std::visit(
      [&visit](auto &kind) -> void
      {
        visit(Based(kind));
        if constexpr (requires { kind.Vertices.Children; })
        {
          Chained(kind.Vertices, visit);
        }
        if constexpr (requires { kind.Attributes.Children; })
        {
          Chained(kind.Attributes, visit);
        }
      },
      one);
}

template <class TableT, class VisitT>
void Tabled(TableT &table, const VisitT &visit)
{
  visit(table.Head.Base);
  for (auto &one : table.Records)
  {
    visit(Based(one));
  }
}

// Calls visit with the Base of every record of the drawing: its table heads and records, its blocks and their
// entities, the entities of model and paper space, and its objects, in that order.
template <class DocumentT, class VisitT>
void Bases(DocumentT &drawing, const VisitT &visit)
{
  Tabled(drawing.Vport, visit);
  Tabled(drawing.Ltype, visit);
  Tabled(drawing.Layer, visit);
  Tabled(drawing.Style, visit);
  Tabled(drawing.View, visit);
  Tabled(drawing.Ucs, visit);
  Tabled(drawing.Appid, visit);
  Tabled(drawing.Dimstyle, visit);
  Tabled(drawing.BlockRecord, visit);
  for (auto &one : drawing.Loose)
  {
    Tabled(one, visit);
  }
  for (auto &one : drawing.Blocks)
  {
    visit(one.Begin.Record.Base);
    visit(one.End.Record.Base);
    for (auto &held : one.Entities)
    {
      Carried(held, visit);
    }
  }
  for (auto &one : drawing.Entities)
  {
    Carried(one, visit);
  }
  for (auto &one : drawing.Objects)
  {
    std::visit([&visit](auto &kind) -> void { visit(Based(kind)); }, one);
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
