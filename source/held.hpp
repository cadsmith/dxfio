#ifndef DXFIO_HELD_HPP
#define DXFIO_HELD_HPP

#include "based.hpp"
#include "objects/io.hpp"

#include "dxfio/document.hpp"
#include "dxfio/entities/sequence.hpp"
#include "dxfio/handle.hpp"

#include <variant>

namespace DxfIO
{
  /**
   *  Calls visit with the handle of each vertex or attribute a sequence holds, and of its SEQEND.
   */
  template <class ChildT, class VisitT>
  void Held(const Entities::Sequence<ChildT> &chain, const VisitT &visit)
  {
    for (const ChildT &child : chain.Children)
    {
      visit(child.Record.Base.Self);
    }
    if (chain.Closing)
    {
      visit((**chain.Closing).Record.Base.Self);
    }
  }

  /**
   *  Calls visit with the handle of an entity, and of the vertices, attributes and SEQEND it
   *  carries.
   */
  template <class VisitT>
  void Held(const Entity &one, const VisitT &visit)
  {
    std::visit(
        [&visit](const auto &kind) -> void
        {
          visit(Based(kind).Self);
          if constexpr (requires { kind.Vertices.Children; })
          {
            Held(kind.Vertices, visit);
          }
          if constexpr (requires { kind.Attributes.Children; })
          {
            Held(kind.Attributes, visit);
          }
        },
        one);
  }

  /**
   *  Calls visit with the handle of a table's head and of each of its records.
   */
  template <class RecordT, class VisitT>
  void Held(const Tables::Table<RecordT> &table, const VisitT &visit)
  {
    visit(table.Head.Base.Self);
    for (const RecordT &one : table.Records)
    {
      visit(Based(one).Self);
    }
  }

  /**
   *  Calls visit with the handle of an object, and with the entities and sort keys of a draw order.
   */
  template <class VisitT>
  void Held(const Object &one, const VisitT &visit)
  {
    visit(Objects::Based(one).Self);
    if (const auto *order = std::get_if<Objects::SortEntsTable>(&one))
    {
      for (const Objects::Sortents::Entry &entry : order->Entries)
      {
        visit(entry.Entity);
        visit(entry.Key.value_or(Handle{}));
      }
    }
  }

  /**
   *  Calls visit with every handle the drawing holds: of its table heads and records, its blocks,
   *  its entities and what they carry, its objects, and the handles a draw order names.
   */
  template <class VisitT>
  void Held(const Document &from, const VisitT &visit)
  {
    Held(from.Vport, visit);
    Held(from.Ltype, visit);
    Held(from.Layer, visit);
    Held(from.Style, visit);
    Held(from.View, visit);
    Held(from.Ucs, visit);
    Held(from.Appid, visit);
    Held(from.Dimstyle, visit);
    Held(from.BlockRecord, visit);
    for (const Tables::Table<Unknown> &one : from.Loose)
    {
      Held(one, visit);
    }
    for (const Blocks::Block &one : from.Blocks)
    {
      visit(one.Begin.Record.Base.Self);
      visit(one.End.Record.Base.Self);
      for (const Entity &held : one.Entities)
      {
        Held(held, visit);
      }
    }
    for (const Entity &one : from.Entities)
    {
      Held(one, visit);
    }
    for (const Object &one : from.Objects)
    {
      Held(one, visit);
    }
  }
} // namespace DxfIO

#endif /* DXFIO_HELD_HPP */
