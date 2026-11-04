#include "../alike.hpp"
#include "../disk.hpp"
#include "../releases.hpp"
#include "../written.hpp"
#include "entities/io.hpp"
#include "file/sections/records.hpp"
#include "heavy.hpp"
#include "seed.hpp"
#include "tags/tags.hpp"

#include "dxfio/load.hpp"
#include "dxfio/save.hpp"

#include <doctest/doctest.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

namespace DxfIO::Entities
{
namespace
{
constexpr std::array Typed{Version::R2000, Version::R2004, Version::R2010, Version::R2018};

constexpr Handle Owner{.Value = 0x1f};

auto Marked(std::string name) -> Group
{
  return Group{.Code = 100, .Value = std::move(name)};
}

auto Flag(std::int64_t value) -> Group
{
  return Group{.Code = 67, .Value = value};
}

auto Layered() -> Group
{
  return Group{.Code = 8, .Value = std::string{"0"}};
}

auto Claimed(std::vector<Group> groups) -> Unknown
{
  Unknown out{};
  out.Groups = std::move(groups);
  Entity one{out};
  Claim(one, Owner);
  REQUIRE(Paper(one));
  out = std::get<Unknown>(one);
  CHECK(out.Base.Owner == Owner);
  return out;
}

auto Section(std::string_view text, std::string_view name = "ENTITIES") -> std::string_view
{
  const std::size_t from = text.find(name);
  REQUIRE(from != std::string_view::npos);
  return text.substr(from, text.find("ENDSEC", from) - from);
}
} // namespace

TEST_CASE("named: a kind's type and a kept entity's own")
{
  CHECK(Named(Entity{Line{}}) == "LINE");
  CHECK(Named(Entity{LwPolyline{}}) == "LWPOLYLINE");
  CHECK(Named(Entity{MText{}}) == "MTEXT");
  Unknown kept{};
  kept.Type = Symbol{"WIPEOUT"};
  CHECK(Named(Entity{kept}) == "WIPEOUT");
}

TEST_CASE("based: a kind's record and a kept entity's alike")
{
  Line ours{};
  ours.Record.Base.Self = Handle{0x2a};
  CHECK(Based(Entity{ours}).Self == Handle{0x2a});
  Unknown kept{};
  kept.Base.Self = Handle{0x2b};
  CHECK(Based(Entity{kept}).Self == Handle{0x2b});
}

TEST_CASE("paper: its flag, a kept entity's by its 67")
{
  Line sheet{};
  CHECK_FALSE(Paper(Entity{sheet}));
  sheet.Record.Paper = true;
  CHECK(Paper(Entity{sheet}));
  Unknown kept{};
  kept.Groups.push_back(Group{.Code = 70, .Value = std::int64_t{1}});
  CHECK_FALSE(Paper(Entity{kept}));
  kept.Groups.push_back(Group{.Code = 67, .Value = std::int64_t{0}});
  CHECK_FALSE(Paper(Entity{kept}));
  kept.Groups.back().Value = std::int64_t{1};
  CHECK(Paper(Entity{kept}));
}

TEST_CASE("paper: a kept entity's 67 of its AcDbEntity part alone")
{
  Unknown kept{};
  kept.Groups = {Marked("AcDbEntity"), Layered(), Marked("AcDbThing"), Flag(1)};
  CHECK_FALSE(Paper(Entity{kept}));
}

TEST_CASE("top: an entity's handle, or a child's above it")
{
  Polyline chain{};
  chain.Record.Base.Self = Handle{0x20};
  CHECK(Top(Entity{chain}) == Handle{0x20});
  chain.Vertices.Children.resize(2);
  chain.Vertices.Children.front().Record.Base.Self = Handle{0x40};
  chain.Vertices.Children.back().Record.Base.Self = Handle{0x30};
  CHECK(Top(Entity{chain}) == Handle{0x40});
  Seqend closing{};
  closing.Record.Base.Self = Handle{0x50};
  chain.Vertices.Closing = Boxed<Seqend>{closing};
  CHECK(Top(Entity{chain}) == Handle{0x50});
  Insert reference{};
  reference.Record.Base.Self = Handle{0x20};
  reference.Attributes.Children.resize(1);
  reference.Attributes.Children.back().Record.Base.Self = Handle{0x60};
  CHECK(Top(Entity{reference}) == Handle{0x60});
  Unknown kept{};
  kept.Base.Self = Handle{0x2b};
  CHECK(Top(Entity{kept}) == Handle{0x2b});
}

TEST_CASE("claim: the owner and the flag, on a child the flag alone")
{
  Line one{};
  Entity held{one};
  Claim(held, Owner);
  CHECK(Based(held).Owner == Owner);
  CHECK(Paper(held));

  const Handle parent{0x40};
  Polyline chain{};
  chain.Record.Base.Self = parent;
  chain.Vertices.Children.resize(2);
  for (Vertex &child : chain.Vertices.Children)
  {
    child.Record.Base.Owner = parent;
  }
  Seqend closing{};
  closing.Record.Base.Owner = parent;
  chain.Vertices.Closing = Boxed<Seqend>{closing};
  Entity whole{chain};
  Claim(whole, Owner);
  const auto &moved = std::get<Polyline>(whole);
  CHECK(moved.Record.Base.Owner == Owner);
  for (const Vertex &child : moved.Vertices.Children)
  {
    CHECK(child.Record.Paper);
    CHECK(child.Record.Base.Owner == parent);
  }
  CHECK((*moved.Vertices.Closing)->Record.Paper);
  CHECK((*moved.Vertices.Closing)->Record.Base.Owner == parent);

  Insert reference{};
  reference.Attributes.Children.resize(1);
  reference.Attributes.Closing = Boxed<Seqend>{};
  Entity tagged{reference};
  Claim(tagged, Owner);
  const auto &carried = std::get<Insert>(tagged);
  CHECK(carried.Attributes.Children.front().Record.Paper);
  CHECK((*carried.Attributes.Closing)->Record.Paper);
}

TEST_CASE("claim: a kept entity's 67 first in its part, or raised there")
{
  CHECK(Claimed({Marked("AcDbEntity"), Layered(), Marked("AcDbThing")}).Groups ==
        std::vector{Marked("AcDbEntity"), Flag(1), Layered(), Marked("AcDbThing")});
  CHECK(Claimed({Marked("AcDbEntity"), Flag(0), Layered()}).Groups ==
        std::vector{Marked("AcDbEntity"), Flag(1), Layered()});
  CHECK(Claimed({Marked("AcDbEntity"), Flag(0), Flag(0)}).Groups ==
        std::vector{Marked("AcDbEntity"), Flag(1), Flag(1)});
  CHECK(Claimed({Layered()}).Groups == std::vector{Flag(1), Layered()});
  CHECK(Claimed({}).Groups == std::vector{Flag(1)});
}

TEST_CASE("claim: another subclass's 67 is not the flag")
{
  CHECK(Claimed({Marked("AcDbEntity"), Marked("AcDbThing"), Flag(0)}).Groups ==
        std::vector{Marked("AcDbEntity"), Flag(1), Marked("AcDbThing"), Flag(0)});
}

TEST_CASE("claim: what AutoCAD wrote in paper space, its flags taken off")
{
  for (const Version release : {Version::R12, Version::R2018})
  {
    CAPTURE(release);
    std::size_t seen = 0;
    for (const Entity &theirs : Heavy(release).Entities)
    {
      const auto *chain = std::get_if<Polyline>(&theirs);
      if (chain == nullptr || !chain->Record.Paper)
      {
        continue;
      }
      ++seen;
      Polyline bare = *chain;
      bare.Record.Paper = false;
      for (Vertex &child : bare.Vertices.Children)
      {
        child.Record.Paper = false;
      }
      (*bare.Vertices.Closing)->Record.Paper = false;
      Entity ours{bare};
      Claim(ours, Based(theirs).Owner);
      CHECK(ours == theirs);
    }
    CHECK(seen == 1);
  }
}

TEST_CASE("entities: every one AutoCAD wrote comes back byte for byte")
{
  for (const std::string_view stem : {"entities", "polylines"})
  {
    for (const Version release : Typed)
    {
      CAPTURE(stem);
      CAPTURE(release);
      const std::string theirs = Acad(Fixture(stem, release));
      const auto read = Load(theirs);
      REQUIRE(read);
      CHECK(read->Issues.empty());
      const auto ours = Tests::Written(read->Document, Options{.Release = release});
      REQUIRE(ours);
      CHECK(Section(*ours) == Section(theirs));
    }
  }
}

TEST_CASE("entities: every chain AutoCAD wrote comes back, blocks and all")
{
  for (const std::string_view stem : {"heavy", "inserts", "texts", "attdefs", "viewports"})
  {
    for (const Version release : Releases)
    {
      CAPTURE(stem);
      CAPTURE(release);
      const std::string theirs = Acad(Fixture(stem, release));
      const auto read = Load(theirs);
      REQUIRE(read);
      CHECK(read->Issues.empty());
      const auto ours = Tests::Written(read->Document, Options{.Release = release});
      REQUIRE(ours);
      CHECK(Alike(Section(*ours), Section(theirs)));
      CHECK(Alike(Section(*ours, "BLOCKS"), Section(theirs, "BLOCKS")));
    }
  }
}

TEST_CASE("entities: R12's come back the same, its reals laid out its way")
{
  const auto read = Load(Acad(Fixture("entities", Version::R12)));
  REQUIRE(read);
  CHECK(read->Issues.empty());
  const List &all = read->Document.Entities;
  REQUIRE(all.size() == 29);
  CHECK(std::get<Line>(all.front()).End == Vector{.X = 100, .Y = 50, .Z = 0});
  CHECK(std::get<Circle>(all.at(2)).Radius == 25);
  const auto ours = Tests::Written(read->Document, Options{.Release = Version::R12});
  REQUIRE(ours);
  const auto again = Load(*ours);
  REQUIRE(again);
  CHECK(again->Document.Entities == all);
}

TEST_CASE("entities: each of a kind, none left unknown")
{
  const auto read = Load(Acad(Fixture("entities", Version::R2018)));
  REQUIRE(read);
  const List &all = read->Document.Entities;
  REQUIRE(all.size() == 29);
  const auto loose =
      std::ranges::count_if(all, [](const Entity &one) -> bool { return std::holds_alternative<Unknown>(one); });
  CHECK(loose == 0);
  CHECK(std::holds_alternative<Viewport>(all.at(all.size() - 2)));
}

TEST_CASE("entities: an entity owned takes a handle, what it holds its own")
{
  Seed handles{Handle{0x40}};
  Polyline polyline{};
  polyline.Vertices.Children.emplace_back();
  polyline.Vertices.Closing = Boxed<Seqend>{Seqend{}};
  Entity one{polyline};
  Own(one, handles, Handle{0x1F});
  const auto &held = std::get<Polyline>(one);
  CHECK(held.Record.Base.Self == Handle{0x40});
  CHECK(held.Record.Base.Owner == Handle{0x1F});
  CHECK(held.Vertices.Children[0].Record.Base.Self == Handle{0x41});
  CHECK(held.Vertices.Children[0].Record.Base.Owner == Handle{0x40});
  CHECK((**held.Vertices.Closing).Record.Base.Owner == Handle{0x40});
}

TEST_CASE("entities: a handle held is kept, a record no kind types is owned")
{
  Seed handles{Handle{0x40}};
  Line line{};
  line.Record.Base.Self = Handle{0x99};
  Entity one{line};
  Own(one, handles, Handle{0x1F});
  CHECK(Based(one).Self == Handle{0x99});
  CHECK(handles.Next() == Handle{0x40});

  Entity loose{Unknown{.Type = Symbol{"MESH"}}};
  Own(loose, handles, Handle{0x1F});
  CHECK(Based(loose).Self == Handle{0x40});
  CHECK(Based(loose).Owner == Handle{0x1F});
}

TEST_CASE("entities: bare, an entity keeps its handle and extended data")
{
  Polyline polyline{};
  polyline.Record.Base.Self = Handle{0x40};
  polyline.Record.Base.Owner = Handle{0x1F};
  polyline.Record.Base.Extended.push_back(Xdata{.Application = "ACAD", .Groups = {}});
  polyline.Record.Base.Extra.push_back(Group{.Code = 90, .Value = std::int64_t{1}});
  polyline.Vertices.Children.emplace_back();
  polyline.Vertices.Children[0].Record.Base.Owner = Handle{0x40};
  Entity one{polyline};
  Bare(one);
  const auto &held = std::get<Polyline>(one);
  CHECK(held.Record.Base.Self == Handle{0x40});
  CHECK_FALSE(held.Record.Base.Owner);
  CHECK(held.Record.Base.Extended.size() == 1);
  CHECK(held.Record.Base.Extra.empty());
  CHECK_FALSE(held.Vertices.Children[0].Record.Base.Owner);

  Unknown kept{.Type = Symbol{"MESH"}};
  kept.Base.Owner = Handle{0x1F};
  Entity loose{kept};
  Bare(loose);
  CHECK_FALSE(Based(loose).Owner);
}

TEST_CASE("entities: bare, its extended data goes when asked")
{
  Line line{};
  line.Record.Base.Extended.push_back(Xdata{.Application = "ACAD", .Groups = {}});
  Entity one{line};
  Bare(one, false);
  CHECK(Based(one).Extended.empty());
}

TEST_CASE("entities: loose, a reactor to what stayed stays")
{
  Line line{};
  line.Record.Base.Reactors = {Handle{0x21}, Handle{0x22}};
  Entity one{line};
  Loose(one, {Handle{0x21}});
  CHECK(Based(one).Reactors == std::vector<Handle>{Handle{0x22}});
}

TEST_CASE("entities: loose, an entity keeps its owner, not what it points at")
{
  Polyline polyline{};
  polyline.Record.Base.Owner = Handle{0x1F};
  polyline.Record.Base.Dictionary = Handle{0x20};
  polyline.Record.Base.Reactors.push_back(Handle{0x21});
  polyline.Vertices.Children.emplace_back();
  polyline.Vertices.Children[0].Record.Base.Reactors.push_back(Handle{0x22});
  Entity one{polyline};
  Loose(one, {Handle{0x21}, Handle{0x22}});
  const auto &held = std::get<Polyline>(one);
  CHECK(held.Record.Base.Owner == Handle{0x1F});
  CHECK_FALSE(held.Record.Base.Dictionary);
  CHECK(held.Record.Base.Reactors.empty());
  CHECK(held.Vertices.Children[0].Record.Base.Reactors.empty());

  Unknown kept{.Type = Symbol{"MESH"}};
  kept.Base.Dictionary = Handle{0x20};
  Entity loose{kept};
  Loose(loose, {});
  CHECK_FALSE(Based(loose).Dictionary);
}
} // namespace DxfIO::Entities

namespace DxfIO::Entities
{
TEST_CASE("entities: in R12 a kind R12 has not stays as it came, its codes its own")
{
  const auto all = Tags::All("  0\nHELIX\n  8\n0\n 70\n     8\n 71\n     3\n"
                             " 10\n1.0\n 20\n2.0\n 30\n0.0\n 40\n0.0\n"
                             "  0\nLINE\n  8\n0\n 10\n0.0\n 20\n0.0\n 30\n0.0\n"
                             " 11\n1.0\n 21\n1.0\n 31\n0.0\n");
  REQUIRE(all);
  const auto records = File::Sections::Records(*all);
  REQUIRE(records);
  const List read = Entities::Read(*records, Fields::Reading{.Release = Version::R12, .Text = Encoding::Utf8()});
  REQUIRE(read.size() == 2);
  CHECK(std::holds_alternative<Unknown>(read.front()));
  CHECK(std::holds_alternative<Line>(read.back()));
}
} // namespace DxfIO::Entities

namespace DxfIO::Entities
{
TEST_CASE("entities: paper space read from an unknown kind's own groups")
{
  const Unknown other{
      .Type = Symbol{"THING"},
      .Base = {},
      .Groups = {Group{.Code = 100, .Value = std::string{"AcDbThing"}}, Group{.Code = 67, .Value = std::int64_t{1}}}};
  CHECK_FALSE(Paper(Entity{other}));
  const Unknown model{
      .Type = Symbol{"THING"},
      .Base = {},
      .Groups = {Group{.Code = 100, .Value = std::string{"AcDbEntity"}}, Group{.Code = 67, .Value = std::int64_t{0}}}};
  CHECK_FALSE(Paper(Entity{model}));
}
} // namespace DxfIO::Entities
