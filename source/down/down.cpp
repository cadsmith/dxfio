#include "down/down.hpp"

#include "drawn.hpp"
#include "empty/classes.hpp"
#include "entities/io.hpp"
#include "header/io.hpp"
#include "masked.hpp"
#include "objects/io.hpp"
#include "past.hpp"
#include "seed.hpp"
#include "seek.hpp"

#include "dxfio/header/header.hpp"

#include <algorithm>
#include <array>
#include <concepts>
#include <format>
#include <iterator>
#include <ranges>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <variant>

namespace DxfIO::Down
{
namespace
{
constexpr int Closed = 1;
constexpr int Generated = 128;

constexpr std::string_view Paper = "*Paper_Space";

constexpr std::array Spaces{
    std::pair<std::string_view, std::string_view>{"*Model_Space", "$MODEL_SPACE"},
    std::pair<std::string_view, std::string_view>{Paper, "$PAPER_SPACE"},
};

constexpr std::array Sections{
    std::string_view{"HEADER"},
    std::string_view{"TABLES"},
    std::string_view{"BLOCKS"},
    std::string_view{"ENTITIES"},
};

constexpr std::array Recent{
    std::string_view{"CELLSTYLEMAP"},
    std::string_view{"TABLECONTENT"},
    std::string_view{"TABLEGEOMETRY"},
};

constexpr std::array Declared{
    std::string_view{"IMAGE"},           std::string_view{"IMAGEDEF"}, std::string_view{"IMAGEDEF_REACTOR"},
    std::string_view{"RASTERVARIABLES"}, std::string_view{"WIPEOUT"},  std::string_view{"WIPEOUTVARIABLES"},
};

constexpr std::array Roots{
    std::string_view{"ACAD_GROUP"},
    std::string_view{"ACAD_IMAGE_DICT"},
    std::string_view{"ACAD_IMAGE_VARS"},
    std::string_view{"ACAD_MLINESTYLE"},
};

constexpr Version Stored = Version::R2013;

struct Narrowing
{
  Version Release{};
  std::vector<Issue> *Issues{nullptr};
  std::set<Handle> *Gone{nullptr};
  Extents Box{};
};

struct Emptying
{
  Version Release{};
  Version From{};
  std::vector<Issue> *Issues{nullptr};
};

template <class KindT, class... KindsT>
inline constexpr bool Among = (std::same_as<KindT, KindsT> || ...);

template <class KindT>
consteval auto First() -> Version
{
  if constexpr (Among<KindT, Entities::Point, Entities::Line, Entities::Circle, Entities::Arc, Entities::Text,
                      Entities::Shape, Entities::Solid, Entities::Trace, Entities::Face3d, Entities::Attdef,
                      Entities::Insert, Entities::Polyline, Entities::Dimension, Entities::LwPolyline>)
  {
    return Version::R10;
  }
  else if constexpr (Among<KindT, Entities::Ellipse, Entities::Spline, Entities::MText, Entities::Leader,
                           Entities::Tolerance, Entities::MLine, Entities::Ray, Entities::XLine, Entities::Viewport,
                           Entities::Region, Entities::Body, Entities::Solid3d>)
  {
    return Version::R13;
  }
  else if constexpr (Among<KindT, Entities::Hatch, Entities::Image, Entities::Wipeout>)
  {
    return Version::R14;
  }
  else
  {
    return Version::R2000;
  }
}

template <class RecordT>
void Bared(Tables::Table<RecordT> &table)
{
  table.Head.Base = {};
  for (RecordT &one : table.Records)
  {
    one.Record.Base = {};
  }
}

template <class RecordT>
void Loosened(Tables::Table<RecordT> &table)
{
  table.Head.Base.Extra.clear();
  table.Head.Base.Dictionary = {};
  table.Head.Base.Reactors.clear();
  for (RecordT &one : table.Records)
  {
    one.Record.Base.Dictionary = {};
    one.Record.Base.Reactors.clear();
  }
}

auto Heavy(const Entities::LwPolyline &one, Seed &handles) -> Entities::Polyline
{
  Entities::Polyline out{};
  out.Record = one.Record;
  out.Position.Z = one.Elevation;
  out.Thickness = one.Thickness;
  out.Flags = Masked(one.Flags, Closed + Generated);
  out.Start = one.Width.value_or(0.0);
  out.End = one.Width.value_or(0.0);
  out.Extrusion = one.Extrusion;
  for (const Entities::LwPolyline::Vertex &vertex : one.Vertices)
  {
    Entities::Vertex child{};
    child.Record = Child(one.Record, handles);
    child.Position = vertex.Position;
    child.Bulge = vertex.Bulge;
    if (vertex.Widths)
    {
      child.Start = vertex.Widths->Start;
      child.End = vertex.Widths->End;
    }
    out.Vertices.Children.push_back(std::move(child));
  }
  out.Vertices.Closing = Boxed{Entities::Seqend{.Record = Child(one.Record, handles)}};
  return out;
}

auto Held(const Entity &one, Version release) -> bool
{
  return std::visit([release]<class KindT>(const KindT & /*kind*/) -> bool { return release >= First<KindT>(); }, one);
}

auto Spare(const Entity &one, Version release) -> bool
{
  return !Held(one, release);
}

auto Tabled(Version release) -> int
{
  if (release >= Version::R2010)
  {
    return 2;
  }
  return release >= Version::R2007 ? 1 : 0;
}

auto Hollow(const Entity &one, const Emptying &with) -> bool
{
  return std::visit(
      [&with]<class KindT>(const KindT &kind) -> bool
      {
        if constexpr (requires { kind.Geometry; })
        {
          const bool lined = !kind.Geometry->Lines.empty();
          return with.Release < Stored ? !lined && kind.Geometry->Guid.has_value() : lined;
        }
        else if constexpr (std::same_as<KindT, Entities::AcadTable>)
        {
          return !kind.Body.empty() && with.Release >= First<KindT>() && Tabled(with.Release) != Tabled(with.From);
        }
        else
        {
          return false;
        }
      },
      one);
}

void Emptied(Entities::List &all, const Emptying &with)
{
  std::erase_if(all,
                [&with](const Entity &one) -> bool
                {
                  const bool hollow = Hollow(one, with);
                  if (hollow && with.Issues != nullptr)
                  {
                    with.Issues->push_back(Issue{
                        .Record = Entities::Based(one).Self,
                        .What = std::format("{} left out, its body is not in {}'s form", Entities::Named(one),
                                            Name(with.Release)),
                    });
                  }
                  return hollow;
                });
}

auto Era(Version release) -> int
{
  if (release >= Version::R2013)
  {
    return 2;
  }
  return release >= Version::R2010 ? 1 : 0;
}

void Lost(std::set<Handle> &gone, Handle one)
{
  if (one)
  {
    gone.insert(one);
  }
}

void Unpointed(Handle &held, const std::set<Handle> &gone)
{
  if (gone.contains(held))
  {
    held = {};
  }
}

void Unstyled(Entities::List &all, const std::set<Handle> &gone)
{
  for (Entity &one : all)
  {
    if (auto *port = std::get_if<Entities::Viewport>(&one))
    {
      Unpointed(port->Port->Style, gone);
    }
  }
}

void Restyled(Document &from, Version release, std::vector<Issue> *issues)
{
  if (Era(release) == Era(Header::Declared(from.Header)))
  {
    return;
  }
  std::set<Handle> gone;
  const auto left = std::erase_if(from.Objects,
                                  [&gone](const Object &one) -> bool
                                  {
                                    const auto *look = std::get_if<Objects::VisualStyle>(&one);
                                    if (look != nullptr)
                                    {
                                      Lost(gone, look->Base.Self);
                                    }
                                    return look != nullptr;
                                  });
  if (left == 0)
  {
    return;
  }
  if (issues != nullptr)
  {
    issues->push_back(Issue{
        .What = std::format("visual styles left out, {} holds them otherwise", Name(release)),
    });
  }
  for (Object &one : from.Objects)
  {
    if (auto *table = std::get_if<Objects::Dictionary>(&one))
    {
      std::erase_if(table->Entries, [&gone](const Objects::Entry &entry) -> bool
                    { return gone.contains(entry.Soft) || gone.contains(entry.Hard); });
    }
  }
  for (Header::Variable &one : from.Header)
  {
    for (Group &value : one.Values)
    {
      if (auto *held = std::get_if<Handle>(&value.Value))
      {
        Unpointed(*held, gone);
      }
    }
  }
  for (Tables::Vport &one : from.Vport.Records)
  {
    Unpointed(one.Style, gone);
  }
  for (Tables::View &one : from.View.Records)
  {
    Unpointed(one.Style, gone);
  }
  Unstyled(from.Entities, gone);
  for (Blocks::Block &one : from.Blocks)
  {
    Unstyled(one.Entities, gone);
  }
}

template <class ToT, class FromT>
auto Moved(const FromT &one) -> ToT
{
  return ToT{.Base = one.Base, .Version = one.Version, .Rows = one.Rows, .Name = one.Name, .Columns = one.Columns};
}

void Classed(std::vector<Raw> &all, std::string_view from, std::string_view to)
{
  for (Raw &one : all)
  {
    for (Group &group : one.Groups)
    {
      if (group.Code == 1 && group.Value == Content{std::string{from}})
      {
        group.Value = std::string{to};
      }
    }
  }
}

void Retabled(Document &from, Version release)
{
  const bool early = release <= Version::R2000;
  for (Object &one : from.Objects)
  {
    if (const auto *data = std::get_if<Objects::DataTable>(&one); data != nullptr && early)
    {
      one = Moved<Objects::AcDbDataTable>(*data);
    }
    else if (const auto *old = std::get_if<Objects::AcDbDataTable>(&one); old != nullptr && !early)
    {
      one = Moved<Objects::DataTable>(*old);
    }
  }
  if (early)
  {
    Classed(from.Classes, "DATATABLE", "ACDBDATATABLE");
  }
  else
  {
    Classed(from.Classes, "ACDBDATATABLE", "DATATABLE");
  }
}

void Told(const Narrowing &with, const Entity &one)
{
  if (with.Issues != nullptr)
  {
    with.Issues->push_back(Issue{
        .Record = Entities::Based(one).Self,
        .What = std::format("{} left out, {} has no such entity", Entities::Named(one), Name(with.Release)),
    });
  }
}

auto Drawn(const Entity &one, Seed &handles, const Narrowing &with) -> Entities::List
{
  return std::visit(
      [&]<class KindT>(const KindT &kind) -> Entities::List
      {
        if constexpr (std::same_as<KindT, Entities::Ellipse> || std::same_as<KindT, Entities::Spline>)
        {
          return {Traced(kind, handles)};
        }
        else if constexpr (std::same_as<KindT, Entities::Ray> || std::same_as<KindT, Entities::XLine>)
        {
          return {Clipped(kind, with.Box)};
        }
        else if constexpr (std::same_as<KindT, Entities::MText>)
        {
          return std::ranges::to<Entities::List>(Lettered(kind, handles) | std::views::as_rvalue);
        }
        else
        {
          return {};
        }
      },
      one);
}

void Redrawn(const Narrowing &with, const Entity &one, const Entity &into)
{
  if (with.Issues != nullptr)
  {
    with.Issues->push_back(Issue{
        .Record = Entities::Based(one).Self,
        .What = std::format("{} drawn as {} for {}", Entities::Named(one), Entities::Named(into), Name(with.Release)),
    });
  }
}

auto Starred(const Entities::Record *one) -> bool
{
  return one->Layer.View().starts_with('*');
}

template <class ChainT>
void Linked(ChainT &chain, std::vector<Entities::Record *> &out)
{
  for (auto &child : chain.Children)
  {
    out.push_back(&child.Record);
  }
  if (chain.Closing)
  {
    out.push_back(&(**chain.Closing).Record);
  }
}

auto Records(Entity &one) -> std::vector<Entities::Record *>
{
  std::vector<Entities::Record *> out;
  std::visit(
      [&out]<class KindT>(KindT &kind) -> void
      {
        if constexpr (requires { kind.Record; })
        {
          out.push_back(&kind.Record);
        }
        if constexpr (std::same_as<KindT, Entities::Polyline>)
        {
          Linked(kind.Vertices, out);
        }
        if constexpr (std::same_as<KindT, Entities::Insert>)
        {
          Linked(kind.Attributes, out);
        }
      },
      one);
  return out;
}

void Grounded(Entity &one, const Narrowing &with)
{
  const std::vector<Entities::Record *> all = Records(one);
  const auto found = std::ranges::find_if(all, Starred);
  if (found == all.end())
  {
    return;
  }
  if (with.Issues != nullptr)
  {
    with.Issues->push_back(Issue{
        .Record = Entities::Based(one).Self,
        .What = std::format("{} moved from layer {} to 0, {} has no layer named by a star", Entities::Named(one),
                            (*found)->Layer.View(), Name(with.Release)),
    });
  }
  for (Entities::Record *record : all)
  {
    if (Starred(record))
    {
      record->Layer = Symbol{"0"};
    }
  }
}

void Narrow(Entities::List &all, Seed &handles, const Narrowing &with)
{
  Entities::List out;
  out.reserve(all.size());
  for (Entity &one : all)
  {
    Entities::List drawn = with.Release < Version::R13 ? Drawn(one, handles, with) : Entities::List{};
    if (!drawn.empty())
    {
      Redrawn(with, one, drawn.front());
      for (Entity &made : drawn)
      {
        out.push_back(std::move(made));
        Entities::Bare(out.back(), with.Release >= Version::R12);
        Grounded(out.back(), with);
      }
      continue;
    }
    if (Spare(one, with.Release))
    {
      Told(with, one);
      Lost(*with.Gone, Entities::Based(one).Self);
      continue;
    }
    const auto *light = std::get_if<Entities::LwPolyline>(&one);
    if (light != nullptr && with.Release < Version::R14)
    {
      out.emplace_back(Heavy(*light, handles));
    }
    else
    {
      out.push_back(std::move(one));
    }
    if (with.Release < Version::R13)
    {
      Entities::Bare(out.back(), with.Release >= Version::R12);
      Grounded(out.back(), with);
    }
  }
  all = std::move(out);
}

void Loosen(Entities::List &all, const std::set<Handle> &gone)
{
  for (Entity &one : all)
  {
    Entities::Loose(one, gone);
  }
}

auto Space(std::string_view name) -> std::string_view
{
  const auto *found = Seek(Spaces, name, &std::pair<std::string_view, std::string_view>::first);
  return found == nullptr ? std::string_view{} : found->second;
}

auto Sheet(std::string_view name) -> bool
{
  return name.starts_with(Paper) && name != Paper;
}

void Blocked(Document &from, Seed &handles, const Narrowing &with)
{
  std::erase_if(from.Blocks, [](const Blocks::Block &one) -> bool { return Sheet(one.Begin.Name); });
  for (Blocks::Block &one : from.Blocks)
  {
    if (with.Release < Version::R13)
    {
      if (const std::string_view space = Space(one.Begin.Name); !space.empty())
      {
        one.Begin.Name = space;
      }
      one.Begin.Record.Base = {};
      one.End.Record.Base = {};
    }
    Narrow(one.Entities, handles, with);
  }
}

auto Listed(const Raw &one) -> bool
{
  const auto name = std::ranges::find(one.Groups, 1, &Group::Code);
  const auto *text = name == one.Groups.end() ? nullptr : std::get_if<std::string>(&name->Value);
  return text != nullptr && std::ranges::contains(Declared, *text);
}

auto Rooted(const Objects::Dictionary &root) -> std::vector<Handle>
{
  std::vector<Handle> out;
  for (const Objects::Entry &one : root.Entries)
  {
    if (std::ranges::contains(Roots, one.Name))
    {
      out.push_back(one.Soft ? one.Soft : one.Hard);
    }
  }
  return out;
}

auto Keeps(const Object &one) -> bool
{
  return std::visit(
      []<class KindT>(const KindT & /*kind*/) -> bool
      {
        return Among<KindT, Objects::Dictionary, Objects::Group, Objects::MLineStyle, Objects::XRecord,
                     Objects::ImageDef, Objects::RasterVariables, Objects::DictionaryVar>;
      },
      one);
}

using Index = std::unordered_map<Handle, const Objects::Dictionary *>;

auto Indexed(const std::vector<Object> &all) -> Index
{
  Index out;
  for (const Object &one : all)
  {
    if (const auto *table = std::get_if<Objects::Dictionary>(&one))
    {
      out.try_emplace(table->Base.Self, table);
    }
  }
  return out;
}

void Reached(const Index &all, std::vector<Handle> waiting, std::set<Handle> &seen)
{
  while (!waiting.empty())
  {
    const Handle one = waiting.back();
    waiting.pop_back();
    if (!seen.insert(one).second)
    {
      continue;
    }
    const auto found = all.find(one);
    if (found == all.end())
    {
      continue;
    }
    for (const Objects::Entry &entry : found->second->Entries)
    {
      waiting.push_back(entry.Soft ? entry.Soft : entry.Hard);
    }
  }
}

void Thinned(Document &from, std::set<Handle> &gone)
{
  std::erase_if(from.Classes, [](const Raw &one) -> bool { return !Listed(one); });
  const auto *root = from.Objects.empty() ? nullptr : std::get_if<Objects::Dictionary>(from.Objects.data());
  std::set<Handle> seen;
  if (root != nullptr)
  {
    seen.insert(root->Base.Self);
    Reached(Indexed(from.Objects), Rooted(*root), seen);
  }
  std::erase_if(from.Objects,
                [&seen, &gone](const Object &one) -> bool
                {
                  const Handle self = Objects::Based(one).Self;
                  const bool spare =
                      !std::holds_alternative<Objects::ImageDefReactor>(one) && (!seen.contains(self) || !Keeps(one));
                  if (spare)
                  {
                    Lost(gone, self);
                  }
                  return spare;
                });
  if (!from.Objects.empty())
  {
    if (auto *held = std::get_if<Objects::Dictionary>(&from.Objects.front()))
    {
      std::erase_if(held->Entries,
                    [](const Objects::Entry &one) -> bool { return !std::ranges::contains(Roots, one.Name); });
    }
  }
}

void Unlisted(std::vector<Handle> &all, const std::set<Handle> &gone)
{
  std::erase_if(all, [&gone](Handle held) -> bool { return gone.contains(held); });
}

void Unreacted(Object &one, const std::set<Handle> &gone)
{
  std::visit(
      [&gone]<class KindT>(KindT &kind) -> void
      {
        if constexpr (requires { kind.Base; })
        {
          Unlisted(kind.Base.Reactors, gone);
        }
        if constexpr (std::same_as<KindT, Objects::Group>)
        {
          Unlisted(kind.Members, gone);
        }
      },
      one);
}

void Unwatched(std::vector<Object> &all, std::set<Handle> &gone)
{
  std::erase_if(all,
                [&gone](const Object &one) -> bool
                {
                  const auto *reactor = std::get_if<Objects::ImageDefReactor>(&one);
                  if (reactor == nullptr || !gone.contains(reactor->Base.Owner))
                  {
                    return false;
                  }
                  Lost(gone, reactor->Base.Self);
                  return true;
                });
  for (Object &one : all)
  {
    Unreacted(one, gone);
  }
}

void Seeded(Document &from, Handle next)
{
  const auto seed = std::ranges::find(from.Header, "$HANDSEED", &Header::Variable::Name);
  if (seed != from.Header.end() && !seed->Values.empty())
  {
    seed->Values.front().Value = next;
  }
}

void Dated(Document &from)
{
  std::set<Handle> gone;
  std::erase_if(from.Objects,
                [&gone](const Object &one) -> bool
                {
                  const auto *loose = std::get_if<Unknown>(&one);
                  if (loose == nullptr || !std::ranges::contains(Recent, loose->Type.View()))
                  {
                    return false;
                  }
                  Lost(gone, loose->Base.Self);
                  return true;
                });
  for (Object &one : from.Objects)
  {
    if (auto *table = std::get_if<Objects::Dictionary>(&one))
    {
      std::erase_if(table->Entries, [&gone](const Objects::Entry &entry) -> bool
                    { return gone.contains(entry.Soft) || gone.contains(entry.Hard); });
    }
  }
}

void Unsectioned(Document &from)
{
  std::erase_if(from.Order, [&from](const std::string &name) -> bool
                { return std::ranges::contains(from.Sections, name, &Section::Name); });
  from.Sections.clear();
}

void Eldest(Document &from)
{
  from.Order.assign(Sections.begin(), Sections.end());
  from.Objects.clear();
  from.Classes.clear();
  from.BlockRecord = {};
  Bared(from.Vport);
  Bared(from.Ltype);
  Bared(from.Layer);
  Bared(from.Style);
  Bared(from.View);
  Bared(from.Ucs);
  Bared(from.Appid);
  Bared(from.Dimstyle);
  std::erase_if(from.Ltype.Records, [](const Tables::Ltype &one) -> bool { return one.Record.Name.starts_with("By"); });
  std::erase_if(from.Layer.Records, [](const Tables::Layer &one) -> bool { return one.Record.Name.starts_with('*'); });
}

void Elder(Document &from, std::set<Handle> &gone)
{
  Thinned(from, gone);
  Loosened(from.Vport);
  Loosened(from.Ltype);
  Loosened(from.Layer);
  Loosened(from.Style);
  Loosened(from.View);
  Loosened(from.Ucs);
  Loosened(from.Appid);
  Loosened(from.Dimstyle);
  Loosened(from.BlockRecord);
  std::erase_if(from.BlockRecord.Records,
                [](const Tables::BlockRecord &one) -> bool { return Sheet(one.Record.Name); });
  for (Tables::BlockRecord &one : from.BlockRecord.Records)
  {
    one.Layout = {};
  }
}

// The drawing as the release holds it, before the CLASS records it needs.
auto Converted(Document from, Version release, std::vector<Issue> *issues) -> Document
{
  if (release >= Version::R2000)
  {
    Restyled(from, release, issues);
    Retabled(from, release);
  }
  const Emptying emptying{.Release = release, .From = Header::Declared(from.Header), .Issues = issues};
  Emptied(from.Entities, emptying);
  for (Blocks::Block &one : from.Blocks)
  {
    Emptied(one.Entities, emptying);
  }
  if (release >= Version::R2007)
  {
    return from;
  }
  if (release >= Version::R2000)
  {
    Dated(from);
    return from;
  }
  std::set<Handle> gone;
  Seed handles{Past(from)};
  const Narrowing with{.Release = release, .Issues = issues, .Gone = &gone, .Box = Bounded(from)};
  Unsectioned(from);
  from.Loose.clear();
  if (release < Version::R13)
  {
    Eldest(from);
  }
  else
  {
    Elder(from, gone);
  }
  if (release < Version::R12)
  {
    from.Appid = {};
    from.Dimstyle = {};
  }
  Blocked(from, handles, with);
  Narrow(from.Entities, handles, with);
  if (release >= Version::R13)
  {
    Unwatched(from.Objects, gone);
    for (Blocks::Block &one : from.Blocks)
    {
      Loosen(one.Entities, gone);
    }
    Loosen(from.Entities, gone);
  }
  Seeded(from, handles.Next());
  return from;
}
} // namespace

// A release declares in CLASS records kinds another did not, LAYOUT and ACDBPLACEHOLDER up to R2000: a drawing
// given a release not its own gets the records that release needs for what it still holds. One kept at its own
// release is written as it came.
auto Narrowed(Document from, Version release, std::vector<Issue> *issues) -> Document
{
  const Version own = Header::Declared(from.Header);
  Document out = Converted(std::move(from), release, issues);
  if (release >= Version::R13 && release != own)
  {
    std::ranges::move(Empty::Undeclared(out, release), std::back_inserter(out.Classes));
  }
  return out;
}
} // namespace DxfIO::Down
