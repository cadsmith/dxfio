#include "dxfio/audit/audit.hpp"

#include "../disk.hpp"
#include "entities/io.hpp"
#include "past.hpp"

#include "dxfio/create.hpp"
#include "dxfio/edit/add.hpp"
#include "dxfio/edit/find.hpp"
#include "dxfio/load.hpp"
#include "dxfio/save.hpp"

#include <doctest/doctest.h>

#include <algorithm>
#include <cstdint>
#include <format>
#include <limits>
#include <string>
#include <variant>

namespace DxfIO::Audit
{
namespace
{
auto Fresh(Version release = Version::R2018) -> Document
{
  return Create(release);
}

auto Lined(Handle self, std::string_view layer = "0") -> Entities::Line
{
  Entities::Line out{};
  out.Record.Base.Self = self;
  out.Record.Base.Owner = Handle{.Value = 0x1f};
  out.Record.Layer = Symbol{layer};
  return out;
}

auto Seed(const Document &drawing) -> Handle
{
  const auto found = std::ranges::find(drawing.Header, "$HANDSEED", &Header::Variable::Name);
  REQUIRE(found != drawing.Header.end());
  return std::get<Handle>(found->Values.front().Value);
}
} // namespace

TEST_CASE("audit: every drawing AutoCAD wrote has no error")
{
  for (const auto &path : Files(Acad()))
  {
    CAPTURE(path.filename().string());
    const std::string text = Bytes(path);
    const auto drawn = Load(text);
    REQUIRE(drawn);
    const Report found = Check(drawn->Document);
    CHECK(found.Errors() == 0);
  }
}

TEST_CASE("audit: a fresh drawing is clean, each release")
{
  for (const Version release : {Version::R12, Version::R2000, Version::R2018})
  {
    CAPTURE(release);
    const Report found = Check(Fresh(release));
    CHECK(found.Findings.empty());
    CHECK(found.Clean());
  }
}

TEST_CASE("audit: a handle used twice, the entity takes a new one")
{
  Document drawing = Fresh();
  const Handle held = drawing.Layer.Records.front().Record.Base.Self;
  drawing.Entities.emplace_back(Lined(held));
  const Report seen = Check(drawing);
  REQUIRE(seen.Errors() == 1);
  CHECK_FALSE(seen.Clean());
  CHECK(seen.Findings.front().Subject == held);
  CHECK_FALSE(seen.Findings.front().Fixed);
  const Report fixed = Repair(drawing);
  CHECK(fixed.Clean());
  CHECK(fixed.Findings.front().Fixed);
  const Handle taken = Entities::Based(drawing.Entities.back()).Self;
  CHECK(taken != held);
  CHECK(Seed(drawing).Value == taken.Value + 1);
  CHECK(Check(drawing).Findings.empty());
}

TEST_CASE("audit: no handle at all, one given, its vertices follow")
{
  Document drawing = Fresh();
  Entities::Polyline heavy{};
  heavy.Record.Base.Owner = Handle{.Value = 0x1f};
  Entities::Vertex point{};
  point.Record.Base.Self = Handle{.Value = 0x500};
  point.Record.Base.Owner = Handle{};
  heavy.Vertices.Children.push_back(point);
  heavy.Vertices.Closing = Boxed{Entities::Seqend{.Record = {.Base = {.Self = Handle{.Value = 0x501}}}}};
  drawing.Entities.emplace_back(heavy);
  const Report fixed = Repair(drawing);
  REQUIRE(fixed.Errors() == 1);
  CHECK(fixed.Findings.front().What == "a POLYLINE has no handle");
  const auto &made = std::get<Entities::Polyline>(drawing.Entities.back());
  CHECK(made.Record.Base.Self);
  CHECK(made.Vertices.Children.front().Record.Base.Owner == made.Record.Base.Self);
}

TEST_CASE("audit: a vertex sharing a handle moves, not what it shares with")
{
  Document drawing = Fresh();
  const Handle held = drawing.Layer.Records.front().Record.Base.Self;
  Entities::Polyline heavy{};
  heavy.Record.Base.Self = Handle{.Value = 0x600};
  Entities::Vertex point{};
  point.Record.Base.Self = held;
  heavy.Vertices.Children.push_back(point);
  heavy.Vertices.Closing = Boxed{Entities::Seqend{.Record = {.Base = {.Self = Handle{.Value = 0x601}}}}};
  drawing.Entities.emplace_back(heavy);
  REQUIRE(Repair(drawing).Errors() == 1);
  const auto &made = std::get<Entities::Polyline>(drawing.Entities.back());
  CHECK(made.Vertices.Children.front().Record.Base.Self != held);
  CHECK(drawing.Layer.Records.front().Record.Base.Self == held);
}

TEST_CASE("audit: R12 carries no handles, none asked")
{
  Document drawing = Fresh(Version::R12);
  drawing.Entities.emplace_back(Lined(Handle{}));
  CHECK(Check(drawing).Findings.empty());
}

TEST_CASE("audit: the records AutoCAD needs, put back with their defaults")
{
  Document drawing = Fresh();
  std::erase_if(drawing.Layer.Records, [](const Tables::Layer &one) -> bool { return one.Record.Name == "0"; });
  std::erase_if(drawing.Ltype.Records,
                [](const Tables::Ltype &one) -> bool { return one.Record.Name == "Continuous"; });
  drawing.Appid.Records.clear();
  const Report seen = Check(drawing);
  CHECK(seen.Errors() == 3);
  CHECK(seen.Findings.front().What == "0 missing");
  const Report fixed = Repair(drawing);
  CHECK(fixed.Clean());
  CHECK(std::ranges::any_of(drawing.Layer.Records,
                            [](const Tables::Layer &one) -> bool { return one.Record.Name == "0"; }));
  CHECK(drawing.Appid.Records.size() == 1);
  CHECK(drawing.Appid.Records.front().Record.Base.Owner == drawing.Appid.Head.Base.Self);
  CHECK(Check(drawing).Findings.empty());
}

TEST_CASE("audit: a record put back names this drawing's own, not another's")
{
  const auto drawn = Load(Bytes(Acad() / "entities-r2018.dxf"));
  REQUIRE(drawn);
  Document drawing = drawn->Document;
  const Tables::Layer zero = *std::ranges::find_if(drawing.Layer.Records, [](const Tables::Layer &one) -> bool
                                                   { return one.Record.Name == "0"; });
  std::erase_if(drawing.Layer.Records, [](const Tables::Layer &one) -> bool { return one.Record.Name == "0"; });
  std::erase_if(drawing.Dimstyle.Records,
                [](const Tables::Dimstyle &one) -> bool { return one.Record.Name == "Standard"; });
  std::erase_if(drawing.Ltype.Records, [](const Tables::Ltype &one) -> bool { return one.Record.Name == "ByBlock"; });
  std::erase_if(drawing.Vport.Records, [](const Tables::Vport &one) -> bool { return one.Record.Name == "*Active"; });
  REQUIRE(Repair(drawing).Clean());
  const auto made = std::ranges::find_if(drawing.Layer.Records,
                                         [](const Tables::Layer &one) -> bool { return one.Record.Name == "0"; });
  REQUIRE(made != drawing.Layer.Records.end());
  CHECK(made->Material == zero.Material);
  CHECK(made->Style == zero.Style);
  const Tables::Dimstyle &style = drawing.Dimstyle.Records.front();
  CHECK(style.Record.Name == "Standard");
  CHECK(style.Dimtxsty == drawing.Style.Records.front().Record.Base.Self);
  CHECK_FALSE(style.Dimblk);
  CHECK(drawing.Vport.Records.front().Record.Name == "*Active");
  CHECK_FALSE(drawing.Vport.Records.front().Style);
  CHECK(drawing.Ltype.Records.front().Record.Name == "ByBlock");
}

TEST_CASE("audit: put back with no object, its layer names none, its style the "
          "new one")
{
  Document drawing = Fresh();
  drawing.Objects.clear();
  drawing.Style.Records.clear();
  drawing.Layer.Records.clear();
  drawing.Dimstyle.Records.clear();
  REQUIRE(Repair(drawing).Clean());
  CHECK_FALSE(drawing.Layer.Records.front().Material);
  CHECK_FALSE(drawing.Layer.Records.front().Style);
  CHECK(drawing.Dimstyle.Records.front().Dimtxsty == drawing.Style.Records.front().Record.Base.Self);
}

TEST_CASE("audit: records put back where AutoCAD looks for them, in order")
{
  const auto drawn = Load(Bytes(Acad() / "entities-r2018.dxf"));
  REQUIRE(drawn);
  Document drawing = drawn->Document;
  REQUIRE(drawing.Appid.Records.size() > 1);
  std::erase_if(drawing.Appid.Records, [](const Tables::Appid &one) -> bool { return one.Record.Name == "ACAD"; });
  std::erase_if(drawing.Ltype.Records,
                [](const Tables::Ltype &one) -> bool { return one.Record.Name == "Continuous"; });
  REQUIRE(Repair(drawing).Clean());
  CHECK(drawing.Appid.Records.front().Record.Name == "ACAD");
  CHECK(drawing.Ltype.Records.at(2).Record.Name == "Continuous");
}

TEST_CASE("audit: R12 records put back carry no handle")
{
  Document drawing = Fresh(Version::R12);
  drawing.Style.Records.clear();
  REQUIRE(Repair(drawing).Errors() == 1);
  REQUIRE(drawing.Style.Records.size() == 1);
  CHECK_FALSE(drawing.Style.Records.front().Record.Base.Self);
}

TEST_CASE("audit: a layer named nowhere is made, its case aside")
{
  Document drawing = Fresh();
  drawing.Entities.emplace_back(Lined(Past(drawing), "Walls"));
  drawing.Entities.emplace_back(Lined(Past(drawing), "WALLS"));
  CHECK(Check(drawing).Errors() == 2);
  const Report fixed = Repair(drawing);
  CHECK(fixed.Errors() == 1);
  CHECK(std::ranges::count_if(drawing.Layer.Records,
                              [](const Tables::Layer &one) -> bool { return one.Record.Name == "Walls"; }) == 1);
  CHECK(Check(drawing).Findings.empty());
}

TEST_CASE("audit: a layer made from layer 0, its material and plot style")
{
  Document drawing = Fresh();
  drawing.Entities.emplace_back(Lined(Past(drawing), "Walls"));
  REQUIRE(Repair(drawing).Errors() == 1);
  const Tables::Layer &made = drawing.Layer.Records.back();
  CHECK(made.Record.Name == "Walls");
  CHECK(made.Material == drawing.Layer.Records.front().Material);
}

TEST_CASE("audit: a layer made from layer 0, wherever layer 0 stands")
{
  Document drawing = Fresh();
  Tables::Layer walls = drawing.Layer.Records.front();
  walls.Record.Name = "Walls";
  walls.Material = Handle{.Value = 0x77};
  walls.Record.Base.Self = Past(drawing);
  drawing.Layer.Records.insert(drawing.Layer.Records.begin(), walls);
  drawing.Entities.emplace_back(Lined(Past(drawing), "Doors"));
  REQUIRE(Repair(drawing).Errors() == 1);
  const Tables::Layer &made = drawing.Layer.Records.back();
  CHECK(made.Record.Name == "Doors");
  CHECK(made.Material == drawing.Layer.Records.at(1).Material);
  CHECK(made.Material != walls.Material);
}

TEST_CASE("audit: a layer with no plot style takes the drawing's default one")
{
  Document drawing = Fresh();
  const Handle style = drawing.Layer.Records.front().Style;
  REQUIRE(style);
  drawing.Layer.Records.front().Style = {};
  const Report found = Check(drawing);
  REQUIRE(found.Findings.size() == 1);
  CHECK(found.Findings.front().What == "layer 0 has no plot style");
  CHECK_FALSE(drawing.Layer.Records.front().Style);
  CHECK(Repair(drawing).Errors() == 1);
  CHECK(drawing.Layer.Records.front().Style == style);
  CHECK(Check(drawing).Findings.empty());
}

TEST_CASE("audit: R12 layers made carry no handle")
{
  Document drawing = Fresh(Version::R12);
  drawing.Entities.emplace_back(Lined(Handle{}, "Walls"));
  REQUIRE(Repair(drawing).Errors() == 1);
  CHECK_FALSE(drawing.Layer.Records.back().Record.Base.Self);
}

TEST_CASE("audit: an unknown linetype falls back to the layer's")
{
  Document drawing = Fresh();
  Entities::Line one = Lined(Past(drawing));
  one.Record.Linetype = Symbol{"Dashed"};
  drawing.Entities.emplace_back(one);
  Entities::Line two = Lined(Past(drawing));
  two.Record.Linetype = Symbol{"BYBLOCK"};
  drawing.Entities.emplace_back(two);
  const Report seen = Check(drawing);
  REQUIRE(seen.Warnings() == 1);
  CHECK(seen.Clean());
  REQUIRE(Repair(drawing).Warnings() == 1);
  CHECK(Entities::Based(drawing.Entities.front()).Self);
  const auto &fixed = std::get<Entities::Line>(drawing.Entities.at(drawing.Entities.size() - 2));
  CHECK(fixed.Record.Linetype == "ByLayer");
}

TEST_CASE("audit: a text style named nowhere becomes the standard one")
{
  Document drawing = Fresh();
  Entities::Text text{};
  text.Record.Base.Self = Past(drawing);
  text.Lettering.Style = Symbol{"Gone"};
  drawing.Entities.emplace_back(text);
  Entities::MText para{};
  para.Record.Base.Self = Handle{.Value = Past(drawing).Value};
  para.Paragraph->Style = Symbol{"Gone"};
  drawing.Entities.emplace_back(para);
  Entities::Text kept{};
  kept.Record.Base.Self = Handle{.Value = Past(drawing).Value};
  kept.Lettering.Style = Symbol{"STANDARD"};
  drawing.Entities.insert(drawing.Entities.begin(), kept);
  REQUIRE(Repair(drawing).Warnings() == 2);
  CHECK(std::get<Entities::Text>(drawing.Entities.front()).Lettering.Style == "STANDARD");
  CHECK(std::get<Entities::Text>(drawing.Entities.at(drawing.Entities.size() - 2)).Lettering.Style == "Standard");
  CHECK(std::get<Entities::MText>(drawing.Entities.back()).Paragraph->Style == "Standard");
}

TEST_CASE("audit: an attribute definition's text style named nowhere becomes the standard one")
{
  Document drawing = Fresh();
  Entities::Attdef size{};
  size.Record.Base.Self = Past(drawing);
  size.Lettering->Style = Symbol{"Gone"};
  drawing.Entities.emplace_back(size);
  CHECK(Check(drawing).Warnings() == 1);
  REQUIRE(Repair(drawing).Warnings() == 1);
  CHECK(std::get<Entities::Attdef>(drawing.Entities.back()).Lettering->Style == "Standard");
}

TEST_CASE("audit: an insert of no block is left out")
{
  Document drawing = Fresh();
  Entities::Insert gone{};
  gone.Record.Base.Self = Past(drawing);
  gone.Block = Symbol{"Nowhere"};
  drawing.Entities.emplace_back(gone);
  Entities::Insert kept{};
  kept.Record.Base.Self = Handle{.Value = gone.Record.Base.Self.Value + 1};
  kept.Block = Symbol{"*model_space"};
  drawing.Entities.emplace_back(kept);
  const std::size_t before = drawing.Entities.size();
  CHECK(Check(drawing).Errors() == 1);
  CHECK(drawing.Entities.size() == before);
  REQUIRE(Repair(drawing).Errors() == 1);
  CHECK(drawing.Entities.size() == before - 1);
}

TEST_CASE("audit: a mesh with a crease short is given one")
{
  Document drawing = Fresh();
  Entities::Mesh mesh{};
  mesh.Record.Base.Self = Past(drawing);
  mesh.Net->Edges = {0, 1, 1, 2};
  mesh.Net->Creases = {0.5};
  drawing.Blocks.front().Entities.emplace_back(mesh);
  Entities::Mesh whole{};
  whole.Record.Base.Self = Handle{.Value = mesh.Record.Base.Self.Value + 1};
  drawing.Blocks.front().Entities.emplace_back(whole);
  drawing.Entities.emplace_back(Lined(Handle{.Value = mesh.Record.Base.Self.Value + 2}));
  const Report seen = Check(drawing);
  REQUIRE(seen.Warnings() == 1);
  CHECK(seen.Findings.front().What == "mesh with 1 creases for 2 edges");
  REQUIRE(Repair(drawing).Warnings() == 1);
  const auto &fixed =
      std::get<Entities::Mesh>(drawing.Blocks.front().Entities.at(drawing.Blocks.front().Entities.size() - 2));
  CHECK(fixed.Net->Creases == std::vector{0.5, 0.0});
}

TEST_CASE("audit: a drawing with no seed to set still repairs, and is given one")
{
  Document drawing = Fresh();
  std::erase_if(drawing.Header, [](const Header::Variable &one) -> bool { return one.Name == "$HANDSEED"; });
  drawing.Appid.Records.clear();
  CHECK(Repair(drawing).Clean());
  CHECK(Seed(drawing) == Past(drawing));
  Document empty = Fresh();
  const auto seed = std::ranges::find(empty.Header, "$HANDSEED", &Header::Variable::Name);
  seed->Values.clear();
  empty.Appid.Records.clear();
  CHECK(Repair(empty).Clean());
  CHECK(Seed(empty) == Past(empty));
}
} // namespace DxfIO::Audit

namespace DxfIO::Audit
{
TEST_CASE("audit: checked only, what has no handle or style stays as it is")
{
  Document drawing = Create(Version::R2018);
  Entities::Text text{};
  text.Record.Base.Self = Past(drawing);
  text.Lettering.Style = Symbol{"Gone"};
  drawing.Entities.emplace_back(text);
  drawing.Entities.emplace_back(Entities::Polyline{});
  const Report seen = Check(drawing);
  CHECK(seen.Errors() == 2);
  CHECK(seen.Warnings() == 1);
  CHECK_FALSE(std::get<Entities::Polyline>(drawing.Entities.back()).Vertices.Closing);
  CHECK(std::ranges::none_of(seen.Findings, &Finding::Fixed));
  CHECK(std::get<Entities::Text>(drawing.Entities.front()).Lettering.Style == "Gone");
}

TEST_CASE("audit: a vertex owned elsewhere keeps its owner when its polyline moves")
{
  Document drawing = Create(Version::R2018);
  const Handle held = drawing.Layer.Records.front().Record.Base.Self;
  const Handle next = Past(drawing);
  Entities::Polyline heavy{};
  heavy.Record.Base.Self = held;
  Entities::Vertex mine{};
  mine.Record.Base.Self = next;
  mine.Record.Base.Owner = held;
  Entities::Vertex other{};
  other.Record.Base.Self = Handle{.Value = next.Value + 1};
  other.Record.Base.Owner = Handle{.Value = 0x77};
  heavy.Vertices.Children = {mine, other};
  heavy.Vertices.Closing = Boxed{Entities::Seqend{.Record = {.Base = {.Self = Handle{.Value = next.Value + 2}}}}};
  drawing.Entities.emplace_back(heavy);
  REQUIRE(Repair(drawing).Errors() == 1);
  const auto &made = std::get<Entities::Polyline>(drawing.Entities.back());
  CHECK(made.Vertices.Children.front().Record.Base.Owner == made.Record.Base.Self);
  CHECK(made.Vertices.Children.back().Record.Base.Owner == Handle{.Value = 0x77});
}
} // namespace DxfIO::Audit

namespace DxfIO::Audit
{
TEST_CASE("audit: with the last handle there is held, the lowest free ones are taken")
{
  constexpr Handle last{.Value = std::numeric_limits<std::uint64_t>::max()};
  Document drawing = Create(Version::R2018);
  drawing.Entities.emplace_back(Lined(last, "Walls"));
  drawing.Entities.emplace_back(Lined(Handle{}));
  drawing.Appid.Records.clear();
  const Report fixed = Repair(drawing);
  CHECK(fixed.Clean());
  CHECK(fixed.Errors() == 3);
  CHECK(std::ranges::all_of(fixed.Findings, &Finding::Fixed));
  CHECK(Entities::Based(drawing.Entities.back()).Self);
  CHECK(drawing.Appid.Records.front().Record.Base.Self);
  CHECK(drawing.Layer.Records.back().Record.Base.Self);
  CHECK(Check(drawing).Findings.empty());
  CHECK(Seed(drawing) == last);
}

TEST_CASE("audit: handles taken up to the last one there is, then from below")
{
  constexpr std::uint64_t last = std::numeric_limits<std::uint64_t>::max();
  Document drawing = Create(Version::R2018);
  drawing.Entities.emplace_back(Lined(Handle{.Value = last - 2}));
  drawing.Entities.emplace_back(Lined(Handle{}));
  drawing.Entities.emplace_back(Lined(Handle{}));
  REQUIRE(Repair(drawing).Clean());
  CHECK(Entities::Based(drawing.Entities.at(1)).Self == Handle{.Value = last - 1});
  CHECK(Entities::Based(drawing.Entities.back()).Self < Handle{.Value = last - 2});
  CHECK(Check(drawing).Findings.empty());
  CHECK(Seed(drawing) == Handle{.Value = last});
}

TEST_CASE("audit: a block record given a new handle takes its block and its entities along")
{
  Document drawing = Create(Version::R2018);
  REQUIRE(Edit::AddBlock(drawing, "A"));
  REQUIRE(Edit::AddBlock(drawing, "B"));
  REQUIRE(Edit::Add(drawing, Entities::Line{}, "A"));
  REQUIRE(Edit::Add(drawing, Entities::Line{}, "B"));
  const Handle shared = Edit::Find(drawing.BlockRecord, "A")->Record.Base.Self;
  Edit::Find(drawing.BlockRecord, "B")->Record.Base.Self = shared;
  Blocks::Block &moved = *Edit::FindBlock(drawing, "B");
  moved.Begin.Record.Base.Owner = shared;
  moved.End.Record.Base.Owner = shared;
  BaseOf(moved.Entities.front()).Owner = shared;
  const Report fixed = Repair(drawing);
  REQUIRE(fixed.Errors() == 1);
  CHECK(fixed.Findings.front().What == std::format("handle {:X} of a block record used twice", shared.Value));
  const Handle now = Edit::Find(drawing.BlockRecord, "B")->Record.Base.Self;
  CHECK(now != shared);
  CHECK(moved.Begin.Record.Base.Owner == now);
  CHECK(moved.End.Record.Base.Owner == now);
  CHECK(BaseOf(moved.Entities.front()).Owner == now);
  const Blocks::Block &kept = *Edit::FindBlock(drawing, "A");
  CHECK(kept.Begin.Record.Base.Owner == shared);
  CHECK(BaseOf(kept.Entities.front()).Owner == shared);
}

TEST_CASE("audit: the paper space record given a new handle takes its entities and its layout along")
{
  Document drawing = Create(Version::R2018);
  REQUIRE(Edit::Add(drawing, Entities::Line{}));
  REQUIRE(Edit::Add(drawing, Entities::Circle{}, Edit::Space::Paper));
  const Tables::BlockRecord &model = *Edit::Find(drawing.BlockRecord, "*Model_Space");
  const Handle shared = model.Record.Base.Self;
  Tables::BlockRecord &paper = *Edit::Find(drawing.BlockRecord, "*Paper_Space");
  paper.Record.Base.Self = shared;
  Blocks::Block &sheet = *Edit::FindBlock(drawing, "*Paper_Space");
  sheet.Begin.Record.Base.Owner = shared;
  sheet.End.Record.Base.Owner = shared;
  BaseOf(drawing.Entities.back()).Owner = shared;
  auto &laid = std::get<Objects::Layout>(*Edit::FindObject(drawing, paper.Layout));
  laid.Block = shared;
  REQUIRE(Repair(drawing).Errors() == 1);
  const Handle now = paper.Record.Base.Self;
  CHECK(now != shared);
  CHECK(sheet.Begin.Record.Base.Owner == now);
  CHECK(sheet.End.Record.Base.Owner == now);
  CHECK(BaseOf(drawing.Entities.back()).Owner == now);
  CHECK(laid.Block == now);
  CHECK(BaseOf(drawing.Entities.front()).Owner == shared);
  CHECK(Edit::FindBlock(drawing, "*Model_Space")->Begin.Record.Base.Owner == shared);
  CHECK(std::get<Objects::Layout>(*Edit::FindObject(drawing, model.Layout)).Block == shared);
}

TEST_CASE("audit: the model space record given a new handle leaves paper space's entities")
{
  Document drawing = Create(Version::R2018);
  REQUIRE(Edit::Add(drawing, Entities::Line{}));
  REQUIRE(Edit::Add(drawing, Entities::Circle{}, Edit::Space::Paper));
  Tables::BlockRecord &model = *Edit::Find(drawing.BlockRecord, "*Model_Space");
  const Handle shared = drawing.Layer.Head.Base.Self;
  model.Record.Base.Self = shared;
  BaseOf(drawing.Entities.front()).Owner = shared;
  BaseOf(drawing.Entities.back()).Owner = shared;
  REQUIRE(Repair(drawing).Errors() == 1);
  const Handle now = model.Record.Base.Self;
  CHECK(now != shared);
  CHECK(BaseOf(drawing.Entities.front()).Owner == now);
  CHECK(BaseOf(drawing.Entities.back()).Owner == shared);
  CHECK(drawing.Layer.Head.Base.Self == shared);
}

TEST_CASE("audit: a table given a new handle takes its records along, not another's")
{
  Document drawing = Create(Version::R2018);
  const Handle shared = drawing.Ltype.Head.Base.Self;
  drawing.Layer.Head.Base.Self = shared;
  for (Tables::Layer &one : drawing.Layer.Records)
  {
    one.Record.Base.Owner = shared;
  }
  Tables::BlockRecord &model = drawing.BlockRecord.Records.front();
  model.Record.Base.Self = drawing.BlockRecord.Head.Base.Self;
  REQUIRE(Repair(drawing).Errors() == 2);
  const Handle now = drawing.Layer.Head.Base.Self;
  CHECK(now != shared);
  CHECK(std::ranges::all_of(drawing.Layer.Records,
                            [now](const Tables::Layer &one) -> bool { return one.Record.Base.Owner == now; }));
  CHECK(std::ranges::all_of(drawing.Ltype.Records,
                            [shared](const Tables::Ltype &one) -> bool { return one.Record.Base.Owner == shared; }));
  CHECK(drawing.BlockRecord.Records.back().Record.Base.Owner == drawing.BlockRecord.Head.Base.Self);
}

TEST_CASE("audit: what an entity carries is checked too, for its layer, linetype and text style")
{
  Document drawing = Create(Version::R2018);
  REQUIRE(Edit::AddBlock(drawing, "Valve"));
  Entities::Attrib number{};
  number.Record.Layer = Symbol{"NoSuchLayer"};
  number.Lettering.Style = Symbol{"NoSuchStyle"};
  Entities::Insert placed{};
  placed.Block = Symbol{"Valve"};
  placed.Follows = true;
  placed.Attributes.Children.push_back(number);
  placed.Attributes.Closing = Boxed{Entities::Seqend{}};
  Entities::Vertex point{};
  point.Record.Linetype = Symbol{"NoSuchType"};
  Entities::Seqend end{};
  end.Record.Layer = Symbol{"Ends"};
  Entities::Polyline heavy{};
  heavy.Vertices.Children.push_back(point);
  heavy.Vertices.Closing = Boxed{end};
  REQUIRE(Edit::Add(drawing, placed));
  REQUIRE(Edit::Add(drawing, heavy));
  const Report seen = Check(drawing);
  CHECK(seen.Errors() == 2);
  CHECK(seen.Warnings() == 2);
  REQUIRE(Repair(drawing).Clean());
  const auto &attributes = std::get<Entities::Insert>(drawing.Entities.front()).Attributes;
  CHECK(attributes.Children.front().Lettering.Style == "Standard");
  const auto &vertices = std::get<Entities::Polyline>(drawing.Entities.back()).Vertices;
  CHECK(vertices.Children.front().Record.Linetype == "ByLayer");
  CHECK(Edit::Find(drawing.Layer, "NoSuchLayer") != nullptr);
  CHECK(Edit::Find(drawing.Layer, "Ends") != nullptr);
  CHECK(Check(drawing).Findings.empty());
}

TEST_CASE("audit: what carries vertices or attributes but no SEQEND is given one, an insert's flagged")
{
  Document drawing = Create(Version::R2018);
  REQUIRE(Edit::AddBlock(drawing, "Valve"));
  Entities::Polyline heavy{};
  heavy.Vertices.Children.resize(2);
  Entities::Insert valve{};
  valve.Block = Symbol{"Valve"};
  valve.Attributes.Children.resize(1);
  REQUIRE(Edit::Add(drawing, heavy));
  REQUIRE(Edit::Add(drawing, valve));
  REQUIRE(Edit::Add(drawing, Entities::Polyline{}));
  auto &open = std::get<Entities::Polyline>(drawing.Entities.at(0));
  open.Vertices.Closing.reset();
  auto &placed = std::get<Entities::Insert>(drawing.Entities.at(1));
  placed.Attributes.Closing.reset();
  placed.Follows = false;
  const Report seen = Check(drawing);
  REQUIRE(seen.Errors() == 3);
  CHECK(seen.Findings.at(0).What == "POLYLINE has no SEQEND");
  CHECK(seen.Findings.at(1).What == "INSERT attributes not marked to follow it");
  CHECK(seen.Findings.at(2).What == "INSERT has no SEQEND");
  REQUIRE(Repair(drawing).Clean());
  REQUIRE(open.Vertices.Closing);
  CHECK((**open.Vertices.Closing).Record.Base.Self);
  CHECK((**open.Vertices.Closing).Record.Base.Owner == open.Record.Base.Self);
  CHECK(placed.Follows);
  REQUIRE(placed.Attributes.Closing);
  CHECK((**placed.Attributes.Closing).Record.Base.Owner == placed.Record.Base.Self);
  CHECK(Check(drawing).Findings.empty());
  const auto saved = Save(drawing);
  REQUIRE(saved);
  const auto read = Load(saved->Bytes);
  REQUIRE(read);
  CHECK(read->Issues.empty());
  CHECK(read->Document.Entities.size() == 3);
}

TEST_CASE("audit: R12 entities with no handle are given a SEQEND with none")
{
  Document drawing = Create(Version::R12);
  REQUIRE(Edit::AddBlock(drawing, "Valve"));
  Entities::Insert valve{};
  valve.Block = Symbol{"Valve"};
  valve.Follows = true;
  valve.Attributes.Children.resize(1);
  drawing.Entities.emplace_back(valve);
  drawing.Entities.emplace_back(Entities::Polyline{});
  REQUIRE(Repair(drawing).Errors() == 2);
  const auto &placed = std::get<Entities::Insert>(drawing.Entities.front());
  REQUIRE(placed.Attributes.Closing);
  CHECK_FALSE((**placed.Attributes.Closing).Record.Base.Self);
  const auto &heavy = std::get<Entities::Polyline>(drawing.Entities.back());
  REQUIRE(heavy.Vertices.Closing);
  CHECK_FALSE((**heavy.Vertices.Closing).Record.Base.Self);
}

TEST_CASE("audit: every table's required records, checked then put back, R12 too")
{
  for (const Version release : {Version::R2018, Version::R12})
  {
    CAPTURE(release);
    Document drawing = Create(release);
    drawing.Layer.Records.clear();
    drawing.Ltype.Records.clear();
    drawing.Style.Records.clear();
    drawing.Dimstyle.Records.clear();
    drawing.Appid.Records.clear();
    drawing.Vport.Records.clear();
    const Report seen = Check(drawing);
    CHECK(seen.Errors() >= 6);
    CHECK(std::ranges::none_of(seen.Findings, &Finding::Fixed));
    CHECK(Repair(drawing).Clean());
    CHECK(Check(drawing).Findings.empty());
  }
}
} // namespace DxfIO::Audit
