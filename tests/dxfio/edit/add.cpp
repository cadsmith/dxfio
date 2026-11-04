#include "dxfio/edit/add.hpp"

#include "dxfio/audit/audit.hpp"
#include "dxfio/create.hpp"
#include "dxfio/edit/find.hpp"
#include "dxfio/load.hpp"
#include "dxfio/save.hpp"

#include <doctest/doctest.h>

#include <algorithm>
#include <cstdint>
#include <limits>
#include <string>
#include <variant>

namespace DxfIO::Edit
{
namespace
{
auto Seed(const Document &drawing) -> Handle
{
  return std::get<Handle>(
      std::ranges::find(drawing.Header, "$HANDSEED", &Header::Variable::Name)->Values.front().Value);
}

void Exhaust(Document &drawing)
{
  std::ranges::find(drawing.Header, "$HANDSEED", &Header::Variable::Name)->Values.front().Value =
      Handle{.Value = std::numeric_limits<std::uint64_t>::max()};
}

auto Recorded(const Document &drawing, std::string_view name) -> Handle
{
  return Find(drawing.BlockRecord, name)->Record.Base.Self;
}

auto Named(std::string_view name) -> Tables::Layer
{
  Tables::Layer out{};
  out.Record.Name = std::string{name};
  return out;
}

template <class RecordT>
auto Called(std::string_view name) -> RecordT
{
  RecordT out{};
  out.Record.Name = std::string{name};
  return out;
}

template <class RecordT>
void Listed(Document &drawing, Tables::Table<RecordT> &table, std::string_view name)
{
  const auto count = table.Records.size();
  const auto self = Add(drawing, Called<RecordT>(name));
  REQUIRE(self);
  REQUIRE(table.Records.size() == count + 1);
  CHECK(table.Records.back().Record.Base.Self == *self);
  CHECK(table.Records.back().Record.Base.Owner == table.Head.Base.Self);
  CHECK(table.Head.Count == static_cast<int>(count + 1));
}
} // namespace

TEST_CASE("add: a polyline's vertices take the flags its kind asks, their own fitting bits kept")
{
  Document drawing = Create();
  Entities::Vertex plain{};
  Entities::Vertex fitted{};
  fitted.Flags = 1 + 32;
  Entities::Vertex first{};
  first.First = 1;
  Entities::Vertex second{};
  second.Second = -2;
  Entities::Vertex third{};
  third.Third = 3;
  Entities::Vertex fourth{};
  fourth.Fourth = 4;
  Entities::Polyline flat{};
  flat.Vertices.Children = {plain, fitted};
  Entities::Polyline spatial{};
  spatial.Flags = 8;
  spatial.Vertices.Children = {plain, fitted};
  Entities::Polyline meshed{};
  meshed.Flags = 16;
  meshed.Vertices.Children = {plain};
  Entities::Polyline faced{};
  faced.Flags = 64;
  faced.Vertices.Children = {plain, first, second, third, fourth};
  for (const auto &one : {flat, spatial, meshed, faced})
  {
    REQUIRE(Add(drawing, one));
  }
  const auto flags = [&drawing](std::size_t at) -> std::vector<int>
  {
    std::vector<int> out;
    for (const Entities::Vertex &one : std::get<Entities::Polyline>(drawing.Entities.at(at)).Vertices.Children)
    {
      out.push_back(one.Flags);
    }
    return out;
  };
  CHECK(flags(0) == std::vector{0, 1});
  CHECK(flags(1) == std::vector{32, 33});
  CHECK(flags(2) == std::vector{64});
  CHECK(flags(3) == std::vector{192, 128, 128, 128, 128});
}

TEST_CASE("add: a record or a block with no name is refused")
{
  Document drawing = Create();
  CHECK(Add(drawing, Tables::Layer{}) == std::unexpected{Failure{0, "a layer needs a name"}});
  CHECK(Add(drawing, Tables::Ltype{}) == std::unexpected{Failure{0, "a linetype needs a name"}});
  CHECK(AddBlock(drawing, "") == std::unexpected{Failure{0, "a block needs a name"}});
  CHECK(Add(drawing, Tables::Vport{}) == std::unexpected{Failure{0, "a viewport needs a name"}});
}

TEST_CASE("edit: an entity added to model space takes the seed and the space's record")
{
  Document drawing = Create();
  const Handle seed = Seed(drawing);
  Entities::Line line{};
  line.End = Vector{.X = 10, .Y = 0, .Z = 0};
  const auto self = Add(drawing, line);
  REQUIRE(self);
  CHECK(*self == seed);
  CHECK(Seed(drawing).Value == seed.Value + 1);
  REQUIRE(drawing.Entities.size() == 1);
  const auto &added = std::get<Entities::Line>(drawing.Entities.front());
  CHECK(added.Record.Base.Self == seed);
  CHECK(added.Record.Base.Owner == Recorded(drawing, "*Model_Space"));
  CHECK_FALSE(added.Record.Paper);
  CHECK(Audit::Check(drawing).Clean());
}

TEST_CASE("edit: an entity added to paper space is flagged so, and owned by its record")
{
  Document drawing = Create();
  const auto self = Add(drawing, Entities::Circle{}, Space::Paper);
  REQUIRE(self);
  const auto &added = std::get<Entities::Circle>(drawing.Entities.front());
  CHECK(added.Record.Paper);
  CHECK(added.Record.Base.Owner == Recorded(drawing, "*Paper_Space"));
  CHECK(Audit::Check(drawing).Clean());
}

TEST_CASE("edit: the vertices an entity carries take handles, owned by the entity")
{
  Document drawing = Create();
  Entities::Polyline polyline{};
  polyline.Vertices.Children.resize(3);
  const auto self = Add(drawing, polyline);
  REQUIRE(self);
  const auto &added = std::get<Entities::Polyline>(drawing.Entities.front());
  for (const Entities::Vertex &one : added.Vertices.Children)
  {
    CHECK(one.Record.Base.Self);
    CHECK(one.Record.Base.Owner == *self);
  }
  CHECK(Seed(drawing).Value == self->Value + 5);
}

TEST_CASE("edit: a polyline added with vertices and no SEQEND is given one, as AutoCAD needs")
{
  Document drawing = Create();
  REQUIRE(Add(drawing, Named("Walls")));
  Entities::Polyline polyline{};
  polyline.Record.Layer = Symbol{"Walls"};
  polyline.Vertices.Children.resize(2);
  const auto self = Add(drawing, polyline, Space::Paper);
  REQUIRE(self);
  const auto &added = std::get<Entities::Polyline>(drawing.Entities.front());
  REQUIRE(added.Vertices.Closing);
  const Entities::Seqend &end = **added.Vertices.Closing;
  CHECK(end.Record.Base.Self);
  CHECK(end.Record.Base.Owner == *self);
  CHECK(end.Record.Layer == "Walls");
  CHECK(end.Record.Paper);
  CHECK(Audit::Check(drawing).Clean());
  const auto saved = Save(drawing);
  REQUIRE(saved);
  const auto read = Load(saved->Bytes);
  REQUIRE(read);
  CHECK(read->Issues.empty());
  CHECK(std::get<Entities::Polyline>(read->Document.Entities.front()).Vertices.Closing);
}

TEST_CASE("edit: an insert added with attributes says they follow, and is given its SEQEND")
{
  Document drawing = Create();
  REQUIRE(AddBlock(drawing, "Valve"));
  Entities::Insert valve{};
  valve.Block = Symbol{"Valve"};
  valve.Attributes.Children.resize(2);
  const auto self = Add(drawing, valve);
  REQUIRE(self);
  const auto &added = std::get<Entities::Insert>(drawing.Entities.front());
  CHECK(added.Follows);
  REQUIRE(added.Attributes.Closing);
  CHECK((**added.Attributes.Closing).Record.Base.Owner == *self);
  CHECK(Audit::Check(drawing).Clean());
  const auto saved = Save(drawing);
  REQUIRE(saved);
  const auto read = Load(saved->Bytes);
  REQUIRE(read);
  CHECK(read->Issues.empty());
  REQUIRE(read->Document.Entities.size() == 1);
  CHECK(std::get<Entities::Insert>(read->Document.Entities.front()).Attributes.Children.size() == 2);
}

TEST_CASE("edit: an insert with no attributes takes no SEQEND, a polyline always one, its own kept")
{
  Document drawing = Create();
  REQUIRE(Add(drawing, Entities::Polyline{}));
  Entities::Insert space{};
  space.Block = Symbol{"*Model_Space"};
  REQUIRE(Add(drawing, space));
  Entities::Polyline closed{};
  closed.Vertices.Children.resize(1);
  closed.Vertices.Closing = Boxed{Entities::Seqend{.Record = {.Base = {.Self = Handle{.Value = 0x7000}}}}};
  REQUIRE(Add(drawing, closed));
  CHECK(std::get<Entities::Polyline>(drawing.Entities.at(0)).Vertices.Closing);
  const auto &bare = std::get<Entities::Insert>(drawing.Entities.at(1));
  CHECK_FALSE(bare.Follows);
  CHECK_FALSE(bare.Attributes.Closing);
  const auto &kept = std::get<Entities::Polyline>(drawing.Entities.at(2));
  CHECK((**kept.Vertices.Closing).Record.Base.Self == Handle{.Value = 0x7000});
  CHECK(Audit::Check(drawing).Clean());
}

TEST_CASE("edit: an entity that brings its own handle keeps it, and the seed moves past it")
{
  Document drawing = Create();
  Entities::Line line{};
  line.Record.Base.Self = Handle{.Value = 0x5000};
  CHECK(Add(drawing, line) == Handle{.Value = 0x5000});
  CHECK(Seed(drawing) == Handle{.Value = 0x5001});
}

TEST_CASE("edit: an entity added to a block is owned by the block's record")
{
  Document drawing = Create();
  const auto begin = AddBlock(drawing, "Door");
  REQUIRE(begin);
  const Blocks::Block *door = FindBlock(drawing, "door");
  REQUIRE(door != nullptr);
  CHECK(door->Begin.Record.Base.Self == *begin);
  const Handle record = Recorded(drawing, "Door");
  CHECK(door->Begin.Record.Base.Owner == record);
  CHECK(door->End.Record.Base.Owner == record);
  CHECK(Find(drawing.BlockRecord, "Door")->Record.Base.Owner == drawing.BlockRecord.Head.Base.Self);

  const auto self = Add(drawing, Entities::Arc{}, "DOOR");
  REQUIRE(self);
  REQUIRE(door->Entities.size() == 1);
  CHECK(BaseOf(door->Entities.front()).Owner == record);
  CHECK(Audit::Check(drawing).Clean());

  const auto saved = Save(drawing);
  REQUIRE(saved);
  const auto read = Load(saved->Bytes);
  REQUIRE(read);
  CHECK(FindEntity(read->Document, *self) != nullptr);
}

TEST_CASE("edit: the spaces' own blocks take entities as the spaces do")
{
  Document drawing = Create();
  REQUIRE(Add(drawing, Entities::Line{}, "*model_space"));
  REQUIRE(Add(drawing, Entities::Line{}, "*PAPER_SPACE"));
  REQUIRE(drawing.Entities.size() == 2);
  CHECK_FALSE(std::get<Entities::Line>(drawing.Entities[0]).Record.Paper);
  CHECK(std::get<Entities::Line>(drawing.Entities[1]).Record.Paper);
  CHECK(FindBlock(drawing, "*Model_Space")->Entities.empty());
}

TEST_CASE("edit: a block of a layout's paper space flags what it takes")
{
  Document drawing = Create();
  REQUIRE(AddBlock(drawing, "*Paper_Space0"));
  Blocks::Block *sheet = FindBlock(drawing, "*Paper_Space0");
  sheet->Begin.Record.Paper = true;
  REQUIRE(Add(drawing, Entities::Line{}, "*Paper_Space0"));
  CHECK(std::get<Entities::Line>(sheet->Entities.front()).Record.Paper);
}

TEST_CASE("edit: a block missing, or one named twice, is refused")
{
  Document drawing = Create();
  const auto missing = Add(drawing, Entities::Line{}, "Door");
  REQUIRE_FALSE(missing);
  CHECK(missing.error().What == "there is no block named Door");
  REQUIRE(AddBlock(drawing, "Door"));
  const auto twice = AddBlock(drawing, "DOOR");
  REQUIRE_FALSE(twice);
  CHECK(twice.error().What == "a block named DOOR is there already");
}

TEST_CASE("edit: an R12 drawing has no block records, and its blocks and entities are owned by none")
{
  Document drawing = Create(Version::R12);
  const auto records = drawing.BlockRecord.Records.size();
  REQUIRE(AddBlock(drawing, "Door"));
  CHECK(drawing.BlockRecord.Records.size() == records);
  CHECK_FALSE(FindBlock(drawing, "Door")->Begin.Record.Base.Owner);
  REQUIRE(Add(drawing, Entities::Line{}));
  CHECK_FALSE(BaseOf(drawing.Entities.back()).Owner);
  const auto saved = Save(drawing, Options{.Release = Version::R12});
  REQUIRE(saved);
  CHECK(Load(saved->Bytes));
}

TEST_CASE("edit: every table takes a record, owned by the table, its count kept")
{
  Document drawing = Create();
  Listed(drawing, drawing.Vport, "*Active");
  Listed(drawing, drawing.Ltype, "Dashed");
  Listed(drawing, drawing.Layer, "Walls");
  Listed(drawing, drawing.Style, "Bold");
  Listed(drawing, drawing.View, "Front");
  Listed(drawing, drawing.Ucs, "Tilted");
  Listed(drawing, drawing.Appid, "DXFIO");
  Listed(drawing, drawing.Dimstyle, "Metric2");
  CHECK(Audit::Check(drawing).Clean());
}

TEST_CASE("edit: a layer takes layer 0's plot style and material, unless it names its own")
{
  Document drawing = Create();
  const Tables::Layer zero = *Find(drawing.Layer, "0");
  REQUIRE(zero.Style);
  REQUIRE(Add(drawing, Named("Walls")));
  CHECK(drawing.Layer.Records.back().Style == zero.Style);
  CHECK(drawing.Layer.Records.back().Material == zero.Material);

  Tables::Layer doors = Named("Doors");
  doors.Style = Handle{.Value = 0x70};
  doors.Material = Handle{.Value = 0x71};
  REQUIRE(Add(drawing, doors));
  CHECK(drawing.Layer.Records.back().Style == Handle{.Value = 0x70});
  CHECK(drawing.Layer.Records.back().Material == Handle{.Value = 0x71});

  drawing.Layer.Records.clear();
  REQUIRE(Add(drawing, Named("Grid")));
  CHECK_FALSE(drawing.Layer.Records.back().Style);
}

TEST_CASE("edit: a record whose name the table holds is refused, but for a viewport")
{
  Document drawing = Create();
  const auto twice = Add(drawing, Named("0"));
  REQUIRE_FALSE(twice);
  CHECK(twice.error().What == "a record named 0 is there already");
  CHECK_FALSE(Add(drawing, Called<Tables::Ltype>("continuous")));
  CHECK(Add(drawing, Called<Tables::Vport>("*ACTIVE")));
}

TEST_CASE("edit: a record that brings its own handle keeps it, and a count past the records stays")
{
  Document drawing = Create();
  Tables::Layer walls = Named("Walls");
  walls.Record.Base.Self = Handle{.Value = 0x6000};
  drawing.Layer.Head.Count = 40;
  CHECK(Add(drawing, walls) == Handle{.Value = 0x6000});
  CHECK(Seed(drawing) == Handle{.Value = 0x6001});
  CHECK(drawing.Layer.Head.Count == 40);
}

TEST_CASE("edit: past the last handle there is, nothing is added")
{
  Document drawing = Create();
  Exhaust(drawing);
  CHECK_FALSE(Add(drawing, Entities::Line{}));
  CHECK_FALSE(Add(drawing, Entities::Line{}, Space::Paper));
  CHECK_FALSE(AddBlock(drawing, "Door"));
  CHECK_FALSE(Add(drawing, Named("Walls")));
  CHECK(drawing.Entities.empty());
  CHECK_FALSE(Find(drawing.Layer, "Walls"));
}
} // namespace DxfIO::Edit
