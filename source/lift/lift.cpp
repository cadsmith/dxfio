#include "lift/lift.hpp"

#include "audit/checks.hpp"
#include "empty/whole.hpp"
#include "entities/io.hpp"
#include "handseed.hpp"
#include "header/io.hpp"
#include "held.hpp"
#include "past.hpp"
#include "seed.hpp"

#include "dxfio/edit/find.hpp"

#include <algorithm>
#include <string_view>
#include <utility>
#include <vector>

namespace DxfIO::Lift
{
namespace
{
constexpr std::string_view Model = "*Model_Space";

constexpr std::string_view Paper = "*Paper_Space";

// The drawing being made, and the handles it takes: those the one lifted leaves free, lowest first.
struct Lifting
{
  Document *Into{};
  Seed *Handles{};
};

// The space block a name stands for, as R12 or R13 names it; empty for any other block.
auto Spaced(std::string_view name) -> std::string_view
{
  if (Edit::Same(name, "$MODEL_SPACE") || Edit::Same(name, Model))
  {
    return Model;
  }
  return Edit::Same(name, "$PAPER_SPACE") || Edit::Same(name, Paper) ? Paper : std::string_view{};
}

auto Free(const Document &from) -> Seed
{
  std::vector<Handle> used;
  Held(from, [&used](Handle one) -> void { used.push_back(one); });
  return Seed{std::move(used)};
}

// Gives a record of the lifted drawing a handle where it has none, and its owner.
void Owned(Base &base, const Lifting &with, Handle owner)
{
  if (!base.Self)
  {
    base.Self = with.Handles->Take();
  }
  base.Owner = owner;
}

auto Record(const Document &drawing, std::string_view block) -> Handle
{
  return Edit::Find(drawing.BlockRecord, block)->Record.Base.Self;
}

// Puts the records of a table into the release's own: one of a name the release has takes its place and its
// handle, any other comes after them with a handle of its own; each is owned by the table and linked as the
// release links it.
template <class RecordT>
void Merged(const Lifting &with, Tables::Table<RecordT> &into, std::vector<RecordT> records)
{
  for (RecordT &one : records)
  {
    RecordT *own = Edit::Find(into, one.Record.Name);
    Base base{};
    base.Self = own == nullptr ? with.Handles->Take() : own->Record.Base.Self;
    base.Owner = into.Head.Base.Self;
    base.Extended = std::move(one.Record.Base.Extended);
    one.Record.Base = std::move(base);
    Audit::Localized(one, *with.Into);
    if (own == nullptr)
    {
      into.Records.push_back(std::move(one));
      continue;
    }
    *own = std::move(one);
  }
  into.Head.Count = std::max(into.Head.Count, static_cast<int>(into.Records.size()));
}

void Tabled(const Lifting &with, Document &from)
{
  Document &into = *with.Into;
  Merged(with, into.Vport, std::move(from.Vport.Records));
  Merged(with, into.Ltype, std::move(from.Ltype.Records));
  Merged(with, into.Layer, std::move(from.Layer.Records));
  Merged(with, into.Style, std::move(from.Style.Records));
  Merged(with, into.View, std::move(from.View.Records));
  Merged(with, into.Ucs, std::move(from.Ucs.Records));
  Merged(with, into.Appid, std::move(from.Appid.Records));
  Merged(with, into.Dimstyle, std::move(from.Dimstyle.Records));
  into.Loose = std::move(from.Loose);
}

// A BLOCK_RECORD for a block of the lifted drawing, owned by the table; its handle.
auto Recorded(const Lifting &with, const std::string &name) -> Handle
{
  Tables::Table<Tables::BlockRecord> &table = with.Into->BlockRecord;
  Tables::BlockRecord record{};
  record.Record.Name = name;
  record.Record.Base.Self = with.Handles->Take();
  record.Record.Base.Owner = table.Head.Base.Self;
  table.Records.push_back(std::move(record));
  table.Head.Count = std::max(table.Head.Count, static_cast<int>(table.Records.size()));
  return table.Records.back().Record.Base.Self;
}

// A block of the lifted drawing: a space block gives its entities to the release's own, any other is given its
// record, and its BLOCK, ENDBLK and entities their handles and owner.
void Blocked(const Lifting &with, Blocks::Block block)
{
  Document &into = *with.Into;
  const std::string_view space = Spaced(block.Begin.Name);
  if (!space.empty())
  {
    Blocks::Block &own = *Edit::FindBlock(into, space);
    const Handle owner = own.Begin.Record.Base.Owner;
    for (Entity &one : block.Entities)
    {
      Entities::Own(one, *with.Handles, owner);
      if (own.Begin.Record.Paper)
      {
        Entities::Claim(one, owner);
      }
      own.Entities.push_back(std::move(one));
    }
    return;
  }
  const Handle owner = Recorded(with, block.Begin.Name);
  Owned(block.Begin.Record.Base, with, owner);
  Owned(block.End.Record.Base, with, owner);
  for (Entity &one : block.Entities)
  {
    Entities::Own(one, *with.Handles, owner);
  }
  into.Blocks.push_back(std::move(block));
}

void Drawn(const Lifting &with, Entities::List all)
{
  Document &into = *with.Into;
  const Handle model = Record(into, Model);
  const Handle paper = Record(into, Paper);
  for (Entity &one : all)
  {
    Entities::Own(one, *with.Handles, Entities::Paper(one) ? paper : model);
    into.Entities.push_back(std::move(one));
  }
}

// The lifted drawing's header variables over the release's own, each in the place of its name.
void Headed(Document &into, std::vector<Header::Variable> all)
{
  for (Header::Variable &one : all)
  {
    const auto found = std::ranges::find(into.Header, one.Name, &Header::Variable::Name);
    if (found == into.Header.end())
    {
      into.Header.push_back(std::move(one));
      continue;
    }
    found->Values = std::move(one.Values);
  }
}

void Seeded(Document &into)
{
  WriteSeed(into, Handle{.Value = Past(into).Value + 1});
}
} // namespace

auto Lifted(Document from, Version release) -> Document
{
  if (Header::Declared(from.Header) >= Version::R13)
  {
    return from;
  }
  Seed handles = Free(from);
  Document into = Empty::Whole(release, handles);
  const Lifting with{.Into = &into, .Handles = &handles};
  Headed(into, std::move(from.Header));
  Tabled(with, from);
  for (Blocks::Block &one : from.Blocks)
  {
    Blocked(with, std::move(one));
  }
  Drawn(with, std::move(from.Entities));
  into.Sections = std::move(from.Sections);
  Seeded(into);
  return into;
}
} // namespace DxfIO::Lift
