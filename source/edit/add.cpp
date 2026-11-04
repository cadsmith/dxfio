#include "dxfio/edit/add.hpp"

#include "entities/io.hpp"
#include "masked.hpp"
#include "seeding.hpp"

#include "dxfio/edit/find.hpp"

#include <algorithm>
#include <concepts>
#include <format>
#include <string>
#include <utility>
#include <variant>

namespace DxfIO::Edit
{
namespace
{
constexpr std::string_view Model = "*Model_Space";
constexpr std::string_view Paper = "*Paper_Space";

auto Owner(const Document &drawing, std::string_view space) -> Handle
{
  const auto *found = Find(drawing.BlockRecord, space);
  return found == nullptr ? Handle{} : found->Record.Base.Self;
}

auto After(Handle one) -> Handle
{
  return Handle{.Value = one.Value + 1};
}

template <class ChildT>
void Sealed(const Entities::Record &parent, Entities::Sequence<ChildT> &chain)
{
  if (chain.Closing)
  {
    return;
  }
  Entities::Record record = parent;
  record.Base = {};
  chain.Closing = Boxed{Entities::Seqend{.Record = std::move(record)}};
}

// Gives a POLYLINE, and an INSERT whose attributes follow it, the SEQEND the file needs after them when they have
// none, on their own layer, and says an INSERT's attributes follow it: else the file reads them on their own. A
// POLYLINE's vertices take the flags its kind asks, else AutoCAD reads them as another kind's.
void Closed(Entity &one)
{
  std::visit(
      [](auto &kind) -> void
      {
        if constexpr (requires { kind.Attributes.Children; })
        {
          kind.Follows = kind.Follows || !kind.Attributes.Children.empty();
          if (kind.Follows)
          {
            Sealed(kind.Record, kind.Attributes);
          }
        }
        if constexpr (requires { kind.Vertices.Children; })
        {
          for (Entities::Vertex &vertex : kind.Vertices.Children)
          {
            vertex.Flags = Entities::Asked(kind, vertex);
          }
          Sealed(kind.Record, kind.Vertices);
        }
      },
      one);
}

constexpr int Attributed = 2;

// Flags a block that takes an ATTDEF as holding attribute definitions, as AutoCAD does, else its inserts ignore them.
void Tagged(Blocks::Block *block, const Entity &one)
{
  if (block != nullptr && Entities::Attributing(one) && Masked(block->Begin.Flags, Attributed) == 0)
  {
    block->Begin.Flags += Attributed;
  }
}

auto Owned(Document &drawing, Entity &one, Handle owner) -> std::expected<Handle, Failure>
{
  return Seeded(drawing).transform(
      [&drawing, &one, owner](Seed handles) -> Handle
      {
        Closed(one);
        Entities::Own(one, handles, owner);
        Moved(drawing, std::max(handles.Next(), After(Entities::Top(one))));
        return BaseOf(one).Self;
      });
}

template <auto Member, class RecordT>
auto Listed(Document &drawing, RecordT one, std::string_view kind) -> std::expected<Handle, Failure>
{
  auto &table = drawing.*Member;
  if (one.Record.Name.empty())
  {
    return std::unexpected{Failure{.Line = 0, .What = std::format("a {} needs a name", kind)}};
  }
  if constexpr (!std::same_as<RecordT, Tables::Vport>)
  {
    if (Find(table, one.Record.Name) != nullptr)
    {
      return std::unexpected{
          Failure{.Line = 0, .What = std::format("a record named {} is there already", one.Record.Name)}};
    }
  }
  return Seeded(drawing).transform(
      [&drawing, &table, &one](Seed handles) -> Handle
      {
        DxfIO::Base &base = one.Record.Base;
        if (!base.Self)
        {
          base.Self = handles.Take();
        }
        base.Owner = table.Head.Base.Self;
        const Handle out = base.Self;
        Moved(drawing, std::max(handles.Next(), After(out)));
        table.Records.push_back(std::move(one));
        table.Head.Count = std::max(table.Head.Count, static_cast<int>(table.Records.size()));
        return out;
      });
}
} // namespace

auto Add(Document &drawing, Entity one, Space space) -> std::expected<Handle, Failure>
{
  const bool paper = space == Space::Paper;
  const Handle owner = Owner(drawing, paper ? Paper : Model);
  return Owned(drawing, one, owner)
      .transform(
          [&drawing, &one, paper, owner](Handle self) -> Handle
          {
            if (paper)
            {
              Entities::Claim(one, owner);
            }
            Tagged(FindBlock(drawing, paper ? Paper : Model), one);
            drawing.Entities.push_back(std::move(one));
            return self;
          });
}

auto Add(Document &drawing, Entity one, std::string_view block) -> std::expected<Handle, Failure>
{
  if (Same(block, Model))
  {
    return Add(drawing, std::move(one), Space::Model);
  }
  if (Same(block, Paper))
  {
    return Add(drawing, std::move(one), Space::Paper);
  }
  Blocks::Block *found = FindBlock(drawing, block);
  if (found == nullptr)
  {
    return std::unexpected{Failure{.Line = 0, .What = std::format("there is no block named {}", block)}};
  }
  const Handle owner = found->Begin.Record.Base.Owner;
  return Owned(drawing, one, owner)
      .transform(
          [found, &one, owner](Handle self) -> Handle
          {
            if (found->Begin.Record.Paper)
            {
              Entities::Claim(one, owner);
            }
            Tagged(found, one);
            found->Entities.push_back(std::move(one));
            return self;
          });
}

auto AddBlock(Document &drawing, std::string_view name) -> std::expected<Handle, Failure>
{
  if (name.empty())
  {
    return std::unexpected{Failure{.Line = 0, .What = "a block needs a name"}};
  }
  if (FindBlock(drawing, name) != nullptr)
  {
    return std::unexpected{Failure{.Line = 0, .What = std::format("a block named {} is there already", name)}};
  }
  const bool recorded = Find(drawing.BlockRecord, Model) != nullptr;
  return Seeded(drawing).transform(
      [&drawing, name, recorded](Seed handles) -> Handle
      {
        Handle owner{};
        if (recorded)
        {
          Tables::BlockRecord record{};
          record.Record.Base.Self = handles.Take();
          record.Record.Base.Owner = drawing.BlockRecord.Head.Base.Self;
          record.Record.Name = std::string{name};
          owner = record.Record.Base.Self;
          drawing.BlockRecord.Records.push_back(std::move(record));
          drawing.BlockRecord.Head.Count =
              std::max(drawing.BlockRecord.Head.Count, static_cast<int>(drawing.BlockRecord.Records.size()));
        }
        Blocks::Block block{};
        block.Begin.Record.Base.Self = handles.Take();
        block.Begin.Record.Base.Owner = owner;
        block.Begin.Name = std::string{name};
        block.End.Record.Base.Self = handles.Take();
        block.End.Record.Base.Owner = owner;
        const Handle out = block.Begin.Record.Base.Self;
        drawing.Blocks.push_back(std::move(block));
        Moved(drawing, handles.Next());
        return out;
      });
}

auto Add(Document &drawing, Tables::Vport one) -> std::expected<Handle, Failure>
{
  return Listed<&Document::Vport>(drawing, std::move(one), "viewport");
}

auto Add(Document &drawing, Tables::Ltype one) -> std::expected<Handle, Failure>
{
  return Listed<&Document::Ltype>(drawing, std::move(one), "linetype");
}

auto Add(Document &drawing, Tables::Layer one) -> std::expected<Handle, Failure>
{
  if (const Tables::Layer *zero = Find(drawing.Layer, "0"))
  {
    one.Style = one.Style ? one.Style : zero->Style;
    one.Material = one.Material ? one.Material : zero->Material;
  }
  return Listed<&Document::Layer>(drawing, std::move(one), "layer");
}

auto Add(Document &drawing, Tables::Style one) -> std::expected<Handle, Failure>
{
  return Listed<&Document::Style>(drawing, std::move(one), "text style");
}

auto Add(Document &drawing, Tables::View one) -> std::expected<Handle, Failure>
{
  return Listed<&Document::View>(drawing, std::move(one), "view");
}

auto Add(Document &drawing, Tables::Ucs one) -> std::expected<Handle, Failure>
{
  return Listed<&Document::Ucs>(drawing, std::move(one), "ucs");
}

auto Add(Document &drawing, Tables::Appid one) -> std::expected<Handle, Failure>
{
  return Listed<&Document::Appid>(drawing, std::move(one), "application");
}

auto Add(Document &drawing, Tables::Dimstyle one) -> std::expected<Handle, Failure>
{
  return Listed<&Document::Dimstyle>(drawing, std::move(one), "dimension style");
}
} // namespace DxfIO::Edit
