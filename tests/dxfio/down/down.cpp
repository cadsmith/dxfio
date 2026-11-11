#include "down/down.hpp"

#include "../written.hpp"
#include "entities/io.hpp"
#include "objects/io.hpp"
#include "past.hpp"

#include "dxfio/audit/audit.hpp"
#include "dxfio/create.hpp"
#include "dxfio/load.hpp"
#include "dxfio/save.hpp"

#include <doctest/doctest.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <format>
#include <fstream>
#include <set>
#include <sstream>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace DxfIO::Down
{
namespace
{
auto Fresh() -> Document
{
  return Create(Version::R2018);
}

auto Bytes(const std::string &name) -> std::string
{
  const std::ifstream file{std::filesystem::path{DXFIO_FIXTURES_DIR} / "acad" / name, std::ios::binary};
  REQUIRE(file.is_open());
  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

auto Light(int flags) -> Entities::LwPolyline
{
  Entities::LwPolyline out{};
  out.Record.Base.Self = Handle{0xA0};
  out.Record.Layer = Symbol{"Walls"};
  out.Flags = flags;
  out.Elevation = 2.0;
  out.Thickness = 3.0;
  out.Width = 0.5;
  out.Vertices.push_back(Entities::LwPolyline::Vertex{
      .Position = Vector{.X = 1, .Y = 2},
      .Widths = Widths{.Start = 0.1, .End = 0.2},
      .Bulge = 0.25,
  });
  out.Vertices.push_back(Entities::LwPolyline::Vertex{.Position = Vector{.X = 3, .Y = 4}});
  return out;
}

// Whether an object is of a kind R13 and R14 have not.
auto Unkept(const Object &one) -> bool
{
  return std::holds_alternative<Objects::Layout>(one) || std::holds_alternative<Objects::Material>(one) ||
         std::holds_alternative<Objects::Scale>(one) || std::holds_alternative<Objects::Placeholder>(one);
}

// How many entities a drawing holding only that one keeps for a release.
auto Kept(const Entity &one, Version release) -> std::size_t
{
  Document from = Fresh();
  from.Entities.push_back(one);
  return Narrowed(from, release, nullptr).Entities.size();
}

auto Bodily(const Entity &one) -> bool
{
  return std::holds_alternative<Entities::Solid3d>(one);
}

auto Coiled(const Entity &one) -> bool
{
  return std::holds_alternative<Entities::Helix>(one);
}

auto Dated(const Object &one) -> bool
{
  return std::holds_alternative<Objects::AcDbDataTable>(one);
}

auto Datum(const Object &one) -> bool
{
  return std::holds_alternative<Objects::DataTable>(one);
}

auto Looking(const Object &one) -> bool
{
  return std::holds_alternative<Objects::VisualStyle>(one);
}

auto Pointing(Handle one) -> bool
{
  return static_cast<bool>(one);
}

auto Watching(const Object &one) -> bool
{
  return std::holds_alternative<Objects::ImageDefReactor>(one);
}

auto Pictured(const Object &one) -> bool
{
  return std::holds_alternative<Objects::ImageDef>(one);
}

// The handles of the images and wipeouts R13 has not, and of the reactors of the images.
auto Pictures(const Document &from) -> std::set<Handle>
{
  std::set<Handle> out;
  for (const Entity &one : from.Entities)
  {
    if (std::holds_alternative<Entities::Image>(one) || std::holds_alternative<Entities::Wipeout>(one))
    {
      out.insert(Entities::Based(one).Self);
    }
  }
  for (const Object &one : from.Objects)
  {
    if (Watching(one))
    {
      out.insert(Objects::Based(one).Self);
    }
  }
  return out;
}

constexpr std::string_view Constraints = "*ADSK_CONSTRAINTS";

// A drawing with a layer named by a star, and entities on it or holding one on it, in its entities and a block.
auto Starred() -> Document
{
  Document out = Fresh();
  Tables::Layer layer = out.Layer.Records.front();
  layer.Record.Name = Constraints;
  out.Layer.Records.push_back(layer);
  layer.Record.Name = "Walls";
  out.Layer.Records.push_back(layer);
  Entities::Line line{};
  line.Record.Base.Self = Handle{0xB0};
  line.Record.Layer = Symbol{Constraints};
  out.Entities.emplace_back(line);
  line.Record.Base.Self = Handle{0xB1};
  line.Record.Layer = Symbol{"Walls"};
  out.Entities.emplace_back(line);
  out.Blocks.push_back(Blocks::Block{});
  out.Blocks.back().Begin.Name = "Door";
  Entities::Insert held{};
  held.Block = Symbol{"Door"};
  Entities::Attrib tag{};
  tag.Record.Layer = Symbol{"Walls"};
  held.Attributes.Children.push_back(tag);
  tag.Record.Layer = Symbol{Constraints};
  held.Attributes.Children.push_back(tag);
  held.Attributes.Closing = Boxed{Entities::Seqend{.Record = tag.Record}};
  out.Entities.emplace_back(held);
  Entities::Insert bare{};
  bare.Block = Symbol{"Door"};
  bare.Record.Layer = Symbol{Constraints};
  out.Entities.emplace_back(bare);
  Entities::LwPolyline light = Light(0);
  light.Record.Layer = Symbol{Constraints};
  out.Blocks.front().Entities.emplace_back(light);
  return out;
}

auto Lifted(const Entities::Record &one) -> bool
{
  return one.Layer.View().starts_with('*');
}

// Whether an entity, or one it holds, is on a layer named by a star.
auto Afloat(const Entity &one) -> bool
{
  if (const auto *chain = std::get_if<Entities::Polyline>(&one))
  {
    return Lifted(chain->Record) || std::ranges::any_of(chain->Vertices.Children, Lifted, &Entities::Vertex::Record);
  }
  if (const auto *held = std::get_if<Entities::Insert>(&one))
  {
    return Lifted(held->Record) || std::ranges::any_of(held->Attributes.Children, Lifted, &Entities::Attrib::Record);
  }
  return Lifted(std::get<Entities::Line>(one).Record);
}

auto Tabular(const Entity &one) -> bool
{
  return std::holds_alternative<Entities::AcadTable>(one);
}

struct Untabled
{
  std::ptrdiff_t Told{};
  std::ptrdiff_t Kept{};
};

// Saves a drawing to a release and reads it back: the tables told left out for their body's form, and those kept.
auto Untable(const Document &from, Version release) -> Untabled
{
  std::vector<Issue> told;
  const auto saved = Tests::Written(from, Options{.Release = release}, &told);
  REQUIRE(saved);
  const auto back = Load(*saved);
  REQUIRE(back);
  const std::string what = std::format("ACAD_TABLE left out, its body is not in {}'s form", Name(release));
  return Untabled{
      .Told = std::ranges::count(told, what, &Issue::What),
      .Kept = std::ranges::count_if(back->Document.Entities, Tabular),
  };
}

auto Unlayered(const Audit::Finding &one) -> bool
{
  return one.What.starts_with("layer");
}

// Whether an object of the drawing still lists one of those handles among its reactors.
auto Watches(const Document &from, const std::set<Handle> &left) -> bool
{
  for (const Object &one : from.Objects)
  {
    for (const Handle held : Objects::Based(one).Reactors)
    {
      if (left.contains(held))
      {
        return true;
      }
    }
  }
  return false;
}
} // namespace

TEST_CASE("down: a drawing for R12 holds no section R12 has not")
{
  Document from = Fresh();
  from.Sections.push_back(Section{.Name = "ACDSDATA", .Groups = {}});
  from.Order.emplace_back("ACDSDATA");
  const Document out = Narrowed(from, Version::R12, nullptr);
  CHECK(out.Order == std::vector<std::string>{"HEADER", "TABLES", "BLOCKS", "ENTITIES"});
  CHECK(out.Classes.empty());
  CHECK(out.Objects.empty());
  CHECK(out.Sections.empty());
  CHECK(out.BlockRecord.Head.Name.empty());
  CHECK(out.BlockRecord.Records.empty());
}

TEST_CASE("down: table records keep no handle, R12 reads none")
{
  const Document out = Narrowed(Fresh(), Version::R12, nullptr);
  CHECK_FALSE(out.Layer.Head.Base.Self);
  REQUIRE_FALSE(out.Layer.Records.empty());
  CHECK_FALSE(out.Layer.Records.front().Record.Base.Self);
  CHECK(out.Style.Records.front().Record.Base.Extended.empty());
}

TEST_CASE("down: ByBlock and ByLayer are no linetypes of R12")
{
  const Document out = Narrowed(Fresh(), Version::R12, nullptr);
  CHECK(out.Ltype.Records.size() == 1);
  CHECK(out.Ltype.Records.front().Record.Name == "Continuous");
}

TEST_CASE("down: the two spaces take R12's names, other sheets are left out")
{
  Document from = Fresh();
  from.Blocks.push_back(Blocks::Block{});
  from.Blocks.back().Begin.Name = "*Paper_Space7";
  from.Blocks.push_back(Blocks::Block{});
  from.Blocks.back().Begin.Name = "Door";
  const Document out = Narrowed(from, Version::R12, nullptr);
  REQUIRE(out.Blocks.size() == 3);
  CHECK(out.Blocks[0].Begin.Name == "$MODEL_SPACE");
  CHECK(out.Blocks[1].Begin.Name == "$PAPER_SPACE");
  CHECK(out.Blocks[2].Begin.Name == "Door");
  CHECK_FALSE(out.Blocks[0].Begin.Record.Base.Self);
}

TEST_CASE("down: R10 has neither applications nor dimension styles")
{
  const Document out = Narrowed(Fresh(), Version::R10, nullptr);
  CHECK(out.Appid.Head.Name.empty());
  CHECK(out.Dimstyle.Head.Name.empty());
  const Document kept = Narrowed(Fresh(), Version::R12, nullptr);
  CHECK_FALSE(kept.Appid.Head.Name.empty());
  CHECK_FALSE(kept.Dimstyle.Head.Name.empty());
}

TEST_CASE("down: a kind R12 has stays, its handle and extended data with it")
{
  Document from = Fresh();
  Entities::Line line{};
  line.Record.Base.Self = Handle{0xB0};
  line.Record.Base.Owner = Handle{0x1F};
  line.Record.Base.Extended.push_back(Xdata{.Application = "ACAD", .Groups = {}});
  line.Record.Base.Reactors.push_back(Handle{0x20});
  from.Entities.emplace_back(line);
  const Document out = Narrowed(from, Version::R12, nullptr);
  REQUIRE(out.Entities.size() == 1);
  const Base &base = Entities::Based(out.Entities.front());
  CHECK(base.Self == Handle{0xB0});
  CHECK_FALSE(base.Owner);
  CHECK(base.Reactors.empty());
  CHECK(base.Extended.size() == 1);
}

TEST_CASE("down: a kind R12 has not is left out and told")
{
  Document from = Fresh();
  Entities::Mesh mesh{};
  mesh.Record.Base.Self = Handle{0xC0};
  from.Entities.emplace_back(mesh);
  from.Entities.emplace_back(Unknown{.Type = Symbol{"MESH"}});
  from.Entities.emplace_back(Entities::Line{});
  std::vector<Issue> issues;
  const Document out = Narrowed(from, Version::R12, &issues);
  REQUIRE(out.Entities.size() == 1);
  CHECK(std::holds_alternative<Entities::Line>(out.Entities.front()));
  REQUIRE(issues.size() == 2);
  CHECK(issues[0].Record == Handle{0xC0});
  CHECK(issues[0].What == "MESH left out, R12 has no such entity");
  CHECK(issues[1].What == "MESH left out, R12 has no such entity");
  CHECK(Narrowed(from, Version::R12, nullptr).Entities.size() == 1);
}

TEST_CASE("down: a lightweight polyline becomes the POLYLINE R12 has")
{
  Document from = Fresh();
  from.Entities.emplace_back(Light(1 + 128));
  const Document out = Narrowed(from, Version::R12, nullptr);
  REQUIRE(out.Entities.size() == 1);
  const auto &heavy = std::get<Entities::Polyline>(out.Entities.front());
  CHECK(heavy.Record.Layer == "Walls");
  CHECK(heavy.Record.Base.Self == Handle{0xA0});
  CHECK(heavy.Flags == 1 + 128);
  CHECK(heavy.Position.Z == 2.0);
  CHECK(heavy.Thickness == 3.0);
  CHECK(heavy.Start == 0.5);
  CHECK(heavy.End == 0.5);
}

TEST_CASE("down: the POLYLINE made of a lightweight one has its vertices, then a SEQEND")
{
  Document from = Fresh();
  from.Entities.emplace_back(Light(1 + 128));
  const Handle first = Handle{Past(from).Value};
  const Document out = Narrowed(from, Version::R12, nullptr);
  const auto &heavy = std::get<Entities::Polyline>(out.Entities.front());
  REQUIRE(heavy.Vertices.Children.size() == 2);
  const Entities::Vertex &bulged = heavy.Vertices.Children[0];
  CHECK(bulged.Position == Vector{.X = 1, .Y = 2});
  CHECK(bulged.Bulge == 0.25);
  CHECK(bulged.Start == 0.1);
  CHECK(bulged.End == 0.2);
  CHECK(bulged.Record.Base.Self == first);
  CHECK_FALSE(heavy.Vertices.Children[1].Start);
  REQUIRE(heavy.Vertices.Closing);
  CHECK(heavy.Vertices.Closing.value_or(Boxed<Entities::Seqend>{})->Record.Base.Self);
}

TEST_CASE("down: flags R12 has not stay out of a polyline made")
{
  Document from = Fresh();
  from.Entities.emplace_back(Light(2));
  const Document out = Narrowed(from, Version::R12, nullptr);
  CHECK(std::get<Entities::Polyline>(out.Entities.front()).Flags == 0);
}

TEST_CASE("down: a polyline with no width takes none")
{
  Document from = Fresh();
  Entities::LwPolyline one = Light(0);
  one.Width = std::nullopt;
  from.Entities.emplace_back(one);
  const Document out = Narrowed(from, Version::R12, nullptr);
  CHECK(std::get<Entities::Polyline>(out.Entities.front()).Start == 0.0);
}

TEST_CASE("down: the seed passes the handles a conversion took")
{
  Document from = Fresh();
  from.Entities.emplace_back(Light(0));
  const Document out = Narrowed(from, Version::R12, nullptr);
  CHECK(Past(out).Value <=
        std::get<Handle>(std::ranges::find(out.Header, "$HANDSEED", &Header::Variable::Name)->Values.front().Value)
            .Value);
}

TEST_CASE("down: a header with no seed is left so")
{
  Document from = Fresh();
  std::erase_if(from.Header, [](const Header::Variable &one) -> bool { return one.Name == "$HANDSEED"; });
  const Document out = Narrowed(from, Version::R12, nullptr);
  CHECK(std::ranges::find(out.Header, "$HANDSEED", &Header::Variable::Name) == out.Header.end());
}

TEST_CASE("down: a seed with no value is left so")
{
  Document from = Fresh();
  std::ranges::find(from.Header, "$HANDSEED", &Header::Variable::Name)->Values.clear();
  const Document out = Narrowed(from, Version::R12, nullptr);
  CHECK(std::ranges::find(out.Header, "$HANDSEED", &Header::Variable::Name)->Values.empty());
}

TEST_CASE("down: an R2018 drawing AutoCAD wrote saves to R12 and reads back")
{
  const auto read = Load(Bytes("entities-r2018.dxf"));
  REQUIRE(read);
  std::vector<Issue> issues;
  const auto saved = Tests::Written(read->Document, Options{.Release = Version::R12}, &issues);
  REQUIRE(saved);
  CHECK(saved->contains("AC1009"));
  CHECK_FALSE(saved->contains("AcDbEntity"));
  const auto again = Load(*saved);
  REQUIRE(again);
  CHECK_FALSE(issues.empty());
}

TEST_CASE("down: a release not the drawing's own gets the CLASS records it needs, LAYOUT up to R2000")
{
  const auto read = Load(Bytes("entities-r2018.dxf"));
  REQUIRE(read);
  CHECK(Narrowed(read->Document, Version::R2018, nullptr) == read->Document);
  const Document narrowed = Narrowed(read->Document, Version::R2000, nullptr);
  const auto declares = [&narrowed](std::string_view name) -> bool
  {
    return std::ranges::any_of(
        narrowed.Classes, [name](const Raw &one) -> bool
        { return !one.Groups.empty() && one.Groups.front() == Group{.Code = 1, .Value = std::string{name}}; });
  };
  CHECK(declares("LAYOUT"));
  CHECK(declares("ACDBPLACEHOLDER"));
  const auto saved = Tests::Written(read->Document, Options{.Release = Version::R2000});
  REQUIRE(saved);
  const auto again = Load(*saved);
  REQUIRE(again);
  for (const Audit::Finding &one : Audit::Check(again->Document).Findings)
  {
    CHECK_FALSE(one.What.starts_with("no CLASS for"));
  }
}

TEST_CASE("down: a drawing already of R12 is written as it stands")
{
  const std::string text = Bytes("empty-r12.dxf");
  const auto read = Load(text);
  REQUIRE(read);
  const auto saved = Tests::Written(read->Document, Options{.Release = Version::R12});
  REQUIRE(saved);
  CHECK(saved->contains("$MODEL_SPACE"));
}

TEST_CASE("down: from R2000 a drawing keeps its sections")
{
  const Document from = Fresh();
  CHECK(Narrowed(from, Version::R2000, nullptr).Order == from.Order);
  CHECK(Narrowed(from, Version::R2013, nullptr) == from);
}

TEST_CASE("down: R13 and R14 keep their sections and handles")
{
  const Document from = Fresh();
  const Document out = Narrowed(from, Version::R14, nullptr);
  CHECK(out.Order == from.Order);
  CHECK(out.Layer.Records.front().Record.Base.Self == from.Layer.Records.front().Record.Base.Self);
  CHECK(out.Blocks.front().Begin.Name == "*Model_Space");
  CHECK(out.Blocks.front().Begin.Record.Base.Self);
  CHECK(out.Classes.empty());
  CHECK(out.Dimstyle.Head.Base.Extra.empty());
}

TEST_CASE("down: R13 and R14 name no section they leave out")
{
  Document from = Fresh();
  const std::vector<std::string> order = from.Order;
  from.Sections.push_back(Section{.Name = "THUMBNAILIMAGE", .Groups = {Group{.Code = 90, .Value = std::int64_t{0}}}});
  from.Order.emplace_back("THUMBNAILIMAGE");
  const Document out = Narrowed(from, Version::R14, nullptr);
  CHECK(out.Sections.empty());
  CHECK(out.Order == order);
  const auto saved = Tests::Written(from, Options{.Release = Version::R13});
  REQUIRE(saved);
  CHECK_FALSE(saved->contains("THUMBNAILIMAGE"));
}

TEST_CASE("down: R13 and R14 hold no object they have no kind for")
{
  const Document out = Narrowed(Fresh(), Version::R14, nullptr);
  CHECK(std::ranges::none_of(out.Objects, Unkept));
  const auto &root = std::get<Objects::Dictionary>(out.Objects.front());
  REQUIRE(root.Entries.size() == 2);
  CHECK(root.Entries[0].Name == "ACAD_GROUP");
  CHECK(root.Entries[1].Name == "ACAD_MLINESTYLE");
  CHECK(out.Objects.size() == 4);
}

TEST_CASE("down: an image's reactor stays with its image")
{
  Document from = Fresh();
  from.Objects.emplace_back(Objects::ImageDefReactor{
      .Base = {.Self = Handle{0x900}, .Owner = Handle{0x901}},
  });
  const Document out = Narrowed(from, Version::R14, nullptr);
  CHECK(std::ranges::any_of(out.Objects, [](const Object &one) -> bool
                            { return std::holds_alternative<Objects::ImageDefReactor>(one); }));
}

TEST_CASE("down: R13 keeps no reactor of an image it leaves out, nor names one")
{
  const auto read = Load(Bytes("rasters-r2018.dxf"));
  REQUIRE(read);
  const std::set<Handle> left = Pictures(read->Document);
  REQUIRE(left.size() == 7);
  const Document out = Narrowed(read->Document, Version::R13, nullptr);
  REQUIRE(std::ranges::any_of(out.Objects, Pictured));
  CHECK(std::ranges::none_of(out.Objects, Watching));
  CHECK_FALSE(Watches(out, left));
  CHECK(std::ranges::count_if(Narrowed(read->Document, Version::R14, nullptr).Objects, Watching) == 2);
}

TEST_CASE("down: R13 takes what it leaves out from a group, and the reactors it owned")
{
  Document from = Fresh();
  Entities::Image image{};
  image.Record.Base.Self = Handle{0x900};
  Entities::Line line{};
  line.Record.Base.Self = Handle{0x901};
  from.Entities.emplace_back(image);
  from.Entities.emplace_back(line);
  from.Entities.emplace_back(Entities::Hatch{});
  Objects::Group group{};
  group.Base.Self = Handle{0x902};
  group.Members = {Handle{0x900}, Handle{0x901}};
  from.Objects.emplace_back(group);
  const auto &root = std::get<Objects::Dictionary>(from.Objects.front());
  const Handle groups = std::ranges::find(root.Entries, "ACAD_GROUP", &Objects::Entry::Name)->Soft;
  for (Object &one : from.Objects)
  {
    auto *table = std::get_if<Objects::Dictionary>(&one);
    if (table != nullptr && table->Base.Self == groups)
    {
      table->Entries.push_back(Objects::Entry{.Name = "*A1", .Soft = {}, .Hard = Handle{0x902}});
      table->Base.Reactors.push_back(Handle{0x903});
    }
  }
  from.Objects.emplace_back(Objects::ImageDefReactor{.Base = {.Self = Handle{0x903}, .Owner = Handle{0x900}}});
  from.Objects.emplace_back(Objects::ImageDefReactor{.Base = {.Self = Handle{0x904}, .Owner = Handle{0x901}}});
  from.Objects.emplace_back(Objects::ImageDefReactor{.Base = {.Self = Handle{0x905}}});
  const Document out = Narrowed(from, Version::R13, nullptr);
  const auto kept = std::ranges::find_if(out.Objects, [](const Object &one) -> bool
                                         { return std::holds_alternative<Objects::Group>(one); });
  REQUIRE(kept != out.Objects.end());
  CHECK(std::get<Objects::Group>(*kept).Members == std::vector<Handle>{Handle{0x901}});
  CHECK(std::ranges::count_if(out.Objects, Watching) == 2);
  CHECK(std::ranges::none_of(out.Objects, [](const Object &one) -> bool
                             { return std::ranges::contains(Objects::Based(one).Reactors, Handle{0x903}); }));
}

TEST_CASE("down: an entry may name its record by a hard pointer")
{
  Document from = Fresh();
  auto &root = std::get<Objects::Dictionary>(from.Objects.front());
  const auto held = std::ranges::find(root.Entries, "ACAD_GROUP", &Objects::Entry::Name);
  REQUIRE(held != root.Entries.end());
  held->Hard = held->Soft;
  held->Soft = {};
  const Document out = Narrowed(from, Version::R14, nullptr);
  CHECK(out.Objects.size() == 4);
}

TEST_CASE("down: a dictionary naming no record leaves nothing behind")
{
  Document from = Fresh();
  auto &root = std::get<Objects::Dictionary>(from.Objects.front());
  const auto held = std::ranges::find(root.Entries, "ACAD_GROUP", &Objects::Entry::Name);
  REQUIRE(held != root.Entries.end());
  held->Soft = Handle{0xFFFF};
  const Document out = Narrowed(from, Version::R14, nullptr);
  CHECK(out.Objects.size() == 3);
}

TEST_CASE("down: dictionaries that name each other end")
{
  Document from = Fresh();
  auto &root = std::get<Objects::Dictionary>(from.Objects.front());
  const auto held = std::ranges::find(root.Entries, "ACAD_GROUP", &Objects::Entry::Name);
  REQUIRE(held != root.Entries.end());
  const Handle group = held->Soft;
  const Handle self = root.Base.Self;
  for (Object &one : from.Objects)
  {
    auto *table = std::get_if<Objects::Dictionary>(&one);
    if (table != nullptr && table->Base.Self == group)
    {
      table->Entries.push_back(Objects::Entry{.Name = "BACK", .Soft = {}, .Hard = self});
    }
  }
  CHECK(Narrowed(from, Version::R14, nullptr).Objects.size() == 4);
}

TEST_CASE("down: a drawing with no root dictionary keeps no object")
{
  Document none = Fresh();
  none.Objects.clear();
  CHECK(Narrowed(none, Version::R14, nullptr).Objects.empty());

  Document odd = Fresh();
  odd.Objects.front() = Objects::Scale{};
  CHECK(Narrowed(odd, Version::R14, nullptr).Objects.empty());
}

TEST_CASE("down: a block record keeps no layout, an extra sheet goes")
{
  Document from = Fresh();
  Tables::BlockRecord sheet{};
  sheet.Record.Name = "*Paper_Space3";
  from.BlockRecord.Records.push_back(sheet);
  from.Blocks.push_back(Blocks::Block{});
  from.Blocks.back().Begin.Name = "*Paper_Space3";
  const Document out = Narrowed(from, Version::R14, nullptr);
  for (const Tables::BlockRecord &one : out.BlockRecord.Records)
  {
    CHECK_FALSE(one.Layout);
    CHECK(one.Record.Name != "*Paper_Space3");
  }
  CHECK(out.Blocks.size() == 2);
}

TEST_CASE("down: each release keeps the kinds it has")
{
  CHECK(Kept(Entities::Hatch{}, Version::R14) == 1);
  CHECK(Kept(Entities::Hatch{}, Version::R13) == 0);
  CHECK(Kept(Entities::Image{}, Version::R14) == 1);
  CHECK(Kept(Entities::Image{}, Version::R13) == 0);
  CHECK(Kept(Entities::Ellipse{}, Version::R13) == 1);
}

TEST_CASE("down: each release keeps the kinds it has, and no more")
{
  CHECK(Kept(Entities::Solid{}, Version::R12) == 1);
  CHECK(Kept(Entities::Viewport{}, Version::R13) == 1);
  CHECK(Kept(Entities::Viewport{}, Version::R12) == 0);
  CHECK(Kept(Entities::AcadTable{}, Version::R14) == 0);
  CHECK(Kept(Unknown{.Type = Symbol{"MESH"}}, Version::R14) == 0);
}

TEST_CASE("down: a lightweight polyline stays from R14, turns before it")
{
  Document from = Fresh();
  from.Entities.emplace_back(Light(0));
  CHECK(std::holds_alternative<Entities::LwPolyline>(Narrowed(from, Version::R14, nullptr).Entities.front()));
  const Document out = Narrowed(from, Version::R13, nullptr);
  const auto &heavy = std::get<Entities::Polyline>(out.Entities.front());
  CHECK(heavy.Vertices.Children[0].Record.Base.Owner == heavy.Record.Base.Self);
}

TEST_CASE("down: R13 keeps an entity's owner, its dictionary and reactors go")
{
  Document from = Fresh();
  Entities::Line line{};
  line.Record.Base.Self = Handle{0xB0};
  line.Record.Base.Owner = Handle{0x1F};
  line.Record.Base.Dictionary = Handle{0x20};
  line.Record.Base.Reactors.push_back(Handle{0x21});
  Entities::Hatch left{};
  left.Record.Base.Self = Handle{0xC1};
  line.Record.Base.Reactors.push_back(Handle{0xC1});
  from.Entities.emplace_back(line);
  from.Entities.emplace_back(left);
  from.Layer.Records.front().Record.Base.Dictionary = Handle{0x22};
  from.Layer.Head.Base.Dictionary = Handle{0x23};
  Objects::Scale scale{};
  scale.Base.Self = Handle{0x21};
  from.Objects.emplace_back(scale);
  const Document out = Narrowed(from, Version::R13, nullptr);
  const Base &base = Entities::Based(out.Entities.front());
  CHECK(base.Owner == Handle{0x1F});
  CHECK_FALSE(base.Dictionary);
  CHECK(base.Reactors.empty());
  CHECK_FALSE(out.Layer.Records.front().Record.Base.Dictionary);
  CHECK_FALSE(out.Layer.Head.Base.Dictionary);
}

TEST_CASE("down: R10 keeps no extended data, it has no applications")
{
  Document from = Fresh();
  Entities::Line line{};
  line.Record.Base.Extended.push_back(Xdata{.Application = "ACAD", .Groups = {}});
  from.Entities.emplace_back(line);
  CHECK(Entities::Based(Narrowed(from, Version::R10, nullptr).Entities.front()).Extended.empty());
}

TEST_CASE("down: R14 keeps the reactors of what it keeps")
{
  Document from = Fresh();
  Entities::Line line{};
  line.Record.Base.Self = Handle{0xB0};
  line.Record.Base.Reactors.push_back(Handle{0xC1});
  Entities::Hatch boundary{};
  boundary.Record.Base.Self = Handle{0xC1};
  from.Entities.emplace_back(line);
  from.Entities.emplace_back(boundary);
  const Document out = Narrowed(from, Version::R14, nullptr);
  CHECK(Entities::Based(out.Entities.front()).Reactors.size() == 1);
}

TEST_CASE("down: what is in a block is held to the same rules")
{
  Document from = Fresh();
  Entities::Line line{};
  line.Record.Base.Dictionary = Handle{0x20};
  from.Blocks.front().Entities.emplace_back(line);
  const Document out = Narrowed(from, Version::R14, nullptr);
  CHECK_FALSE(Entities::Based(out.Blocks.front().Entities.front()).Dictionary);
}

TEST_CASE("down: a class stays only for a kind R13 and R14 declare")
{
  Document from = Fresh();
  from.Classes.clear();
  from.Classes.push_back(Raw{
      .Type = "CLASS",
      .Groups = {Group{.Code = 1, .Value = std::string{"IMAGE"}}},
  });
  from.Classes.push_back(Raw{
      .Type = "CLASS",
      .Groups = {Group{.Code = 1, .Value = std::string{"SCALE"}}},
  });
  from.Classes.push_back(Raw{
      .Type = "CLASS",
      .Groups = {Group{.Code = 1, .Value = std::int64_t{1}}},
  });
  from.Classes.push_back(Raw{.Type = "CLASS", .Groups = {}});
  const Document out = Narrowed(from, Version::R14, nullptr);
  REQUIRE(out.Classes.size() == 1);
  CHECK(std::get<std::string>(out.Classes.front().Groups.front().Value) == "IMAGE");
}

TEST_CASE("down: objects newer than R2007 go below it, their entries too")
{
  Document from = Fresh();
  Unknown table{.Type = Symbol{"TABLECONTENT"}};
  table.Base.Self = Handle{0x900};
  Unknown other{.Type = Symbol{"SOMETHING"}};
  other.Base.Self = Handle{0x901};
  from.Objects.emplace_back(table);
  from.Objects.emplace_back(other);
  auto &root = std::get<Objects::Dictionary>(from.Objects.front());
  root.Entries.push_back(Objects::Entry{.Name = "NEW", .Soft = Handle{0x900}, .Hard = {}});
  root.Entries.push_back(Objects::Entry{.Name = "OLD", .Soft = {}, .Hard = Handle{0x900}});
  const Document out = Narrowed(from, Version::R2004, nullptr);
  CHECK(out.Objects.size() == from.Objects.size() - 1);
  CHECK(std::get<Objects::Dictionary>(out.Objects.front()).Entries.size() ==
        std::get<Objects::Dictionary>(from.Objects.front()).Entries.size() - 2);
  CHECK(Narrowed(from, Version::R2007, nullptr) == from);
}

TEST_CASE("down: an object gone with no handle takes no entry with it")
{
  Document from = Fresh();
  from.Objects.emplace_back(Unknown{.Type = Symbol{"TABLECONTENT"}});
  const std::size_t entries = std::get<Objects::Dictionary>(from.Objects.front()).Entries.size();
  REQUIRE(entries > 0);
  const Document out = Narrowed(from, Version::R2004, nullptr);
  CHECK(out.Objects.size() == from.Objects.size() - 1);
  CHECK(std::get<Objects::Dictionary>(out.Objects.front()).Entries.size() == entries);
}

TEST_CASE("down: a visual style gone with no handle is told, and takes no entry")
{
  Document from = Fresh();
  from.Objects.emplace_back(Objects::VisualStyle{});
  const std::size_t entries = std::get<Objects::Dictionary>(from.Objects.front()).Entries.size();
  std::vector<Issue> told;
  const Document out = Narrowed(from, Version::R2010, &told);
  CHECK(out.Objects.size() == from.Objects.size() - 1);
  CHECK(std::get<Objects::Dictionary>(out.Objects.front()).Entries.size() == entries);
  REQUIRE(told.size() == 1);
  CHECK(told.front().What == "visual styles left out, R2010 holds them otherwise");
}

TEST_CASE("down: told which release has no such entity")
{
  Document from = Fresh();
  from.Entities.emplace_back(Entities::Hatch{});
  std::vector<Issue> issues;
  const Document out = Narrowed(from, Version::R13, &issues);
  REQUIRE(issues.size() == 1);
  CHECK(issues.front().What == "HATCH left out, R13 has no such entity");
}

TEST_CASE("down: a body kept in ACDSDATA goes with it, each one told")
{
  const auto drawn = Load(Bytes("kinds-r2018.dxf"));
  REQUIRE(drawn);
  std::vector<Issue> told;
  const auto saved = Tests::Written(drawn->Document, Options{.Release = Version::R2010}, &told);
  REQUIRE(saved);
  REQUIRE(told.size() == 13);
  CHECK(told.front().What == "visual styles left out, R2010 holds them otherwise");
  CHECK(told.at(1).What == "3DSOLID left out, its body is not in R2010's form");
  const auto back = Load(*saved);
  REQUIRE(back);
  CHECK(std::ranges::none_of(back->Document.Entities, Bodily));
  CHECK(std::ranges::any_of(back->Document.Entities, Coiled));
}

TEST_CASE("down: a body in lines has no form from R2013, it goes the same way")
{
  const auto drawn = Load(Bytes("kinds-r2007.dxf"));
  REQUIRE(drawn);
  std::vector<Issue> told;
  const auto saved = Tests::Written(drawn->Document, Options{.Release = Version::R2018}, &told);
  REQUIRE(saved);
  REQUIRE(told.size() == 13);
  CHECK(told.at(1).What == "3DSOLID left out, its body is not in R2018's form");
}

TEST_CASE("down: a body in lines stays while its release holds lines")
{
  const auto drawn = Load(Bytes("kinds-r2007.dxf"));
  REQUIRE(drawn);
  std::vector<Issue> told;
  REQUIRE(Tests::Written(drawn->Document, Options{.Release = Version::R2000}, &told));
  CHECK(told.empty());
}

TEST_CASE("down: a body with nothing of its own stays, whatever the release")
{
  Document drawing = Fresh();
  drawing.Entities.emplace_back(Entities::Solid3d{});
  const Document one = Narrowed(drawing, Version::R2010, nullptr);
  CHECK(one.Entities.size() == drawing.Entities.size());
  const Document two = Narrowed(drawing, Version::R2018, nullptr);
  CHECK(two.Entities.size() == drawing.Entities.size());
}

TEST_CASE("down: a body in a block goes as well, no list of issues asked")
{
  Document drawing = Fresh();
  Entities::Solid3d solid{};
  solid.Geometry->Guid = "{0}";
  drawing.Blocks.front().Entities.emplace_back(solid);
  const Document one = Narrowed(drawing, Version::R2010, nullptr);
  CHECK(one.Blocks.front().Entities.empty());
}

TEST_CASE("down: R12 takes no layer named by a star, its lights go first")
{
  const auto drawn = Load(Bytes("kinds-r2018.dxf"));
  REQUIRE(drawn);
  const Document one = Narrowed(drawn->Document, Version::R12, nullptr);
  CHECK(std::ranges::none_of(one.Layer.Records,
                             [](const Tables::Layer &layer) -> bool { return layer.Record.Name.starts_with('*'); }));
}

TEST_CASE("down: R12 moves what is on a layer named by a star to layer 0, each told")
{
  std::vector<Issue> told;
  const Document out = Narrowed(Starred(), Version::R12, &told);
  REQUIRE(told.size() == 4);
  CHECK(told.front().What == "POLYLINE moved from layer *ADSK_CONSTRAINTS to 0, R12 has no layer named by a star");
  CHECK(told.at(1).Record == Handle{0xB0});
  CHECK(told.at(1).What == "LINE moved from layer *ADSK_CONSTRAINTS to 0, R12 has no layer named by a star");
  CHECK(told.at(2).What == "INSERT moved from layer *ADSK_CONSTRAINTS to 0, R12 has no layer named by a star");
  CHECK(std::ranges::none_of(out.Entities, Afloat));
  CHECK(std::ranges::none_of(out.Blocks.front().Entities, Afloat));
  CHECK(std::get<Entities::Line>(out.Entities.at(1)).Record.Layer == "Walls");
}

TEST_CASE("down: R10 moves them the same, untold, and the audit finds every layer")
{
  const Document from = Starred();
  CHECK(Narrowed(from, Version::R10, nullptr).Entities == Narrowed(from, Version::R12, nullptr).Entities);
  const auto saved = Tests::Written(from, Options{.Release = Version::R12});
  REQUIRE(saved);
  const auto back = Load(*saved);
  REQUIRE(back);
  CHECK(std::ranges::none_of(Audit::Check(back->Document).Findings, Unlayered));
}

TEST_CASE("down: a table whose cells another release lays out otherwise is left out, told")
{
  const auto read = Load(Bytes("schedules-r2018.dxf"));
  REQUIRE(read);
  REQUIRE(std::ranges::count_if(read->Document.Entities, Tabular) == 3);
  const Untabled seven = Untable(read->Document, Version::R2007);
  CHECK(seven.Told == 3);
  CHECK(seven.Kept == 0);
  const Untabled four = Untable(read->Document, Version::R2004);
  CHECK(four.Told == 3);
  CHECK(four.Kept == 0);
  const Untabled two = Untable(read->Document, Version::R2000);
  CHECK(two.Told == 3);
  CHECK(two.Kept == 0);
}

TEST_CASE("down: a table whose cells are laid out the same stays, or one with none")
{
  const auto read = Load(Bytes("schedules-r2018.dxf"));
  REQUIRE(read);
  CHECK(std::ranges::count_if(Narrowed(read->Document, Version::R2010, nullptr).Entities, Tabular) == 3);
  const auto old = Load(Bytes("schedules-r2000.dxf"));
  REQUIRE(old);
  CHECK(std::ranges::count_if(Narrowed(old->Document, Version::R2004, nullptr).Entities, Tabular) == 3);
  Document drawing = Fresh();
  drawing.Entities.emplace_back(Entities::AcadTable{});
  CHECK(std::ranges::count_if(Narrowed(drawing, Version::R2007, nullptr).Entities, Tabular) == 1);
  std::vector<Issue> told;
  CHECK(Narrowed(read->Document, Version::R14, &told).Entities.size() < read->Document.Entities.size());
  CHECK(std::ranges::count(told, "ACAD_TABLE left out, R14 has no such entity", &Issue::What) == 3);
}

TEST_CASE("down: a data table takes R2000's name, and back from it")
{
  const auto drawn = Load(Bytes("schedules-r2004.dxf"));
  REQUIRE(drawn);
  const Document old = Narrowed(drawn->Document, Version::R2000, nullptr);
  CHECK(std::ranges::count_if(old.Objects, Dated) == 3);
  CHECK(std::ranges::none_of(old.Objects, Datum));
  const auto saved = Tests::Written(old, Options{.Release = Version::R2000});
  REQUIRE(saved);
  CHECK(saved->find("\r\n  1\r\nACDBDATATABLE\r\n") != std::string::npos);
  CHECK(saved->find("\r\n  1\r\nDATATABLE\r\n") == std::string::npos);
}

TEST_CASE("down: a data table of R2000 takes its newer name from R2004")
{
  const auto back = Load(Bytes("schedules-r2000.dxf"));
  REQUIRE(back);
  const Document lifted = Narrowed(back->Document, Version::R2004, nullptr);
  CHECK(std::ranges::count_if(lifted.Objects, Datum) == 3);
}

TEST_CASE("down: visual styles of another form are left out, all that names "
          "them cleared")
{
  const auto drawn = Load(Bytes("kinds-r2004.dxf"));
  REQUIRE(drawn);
  std::vector<Issue> told;
  const Document lifted = Narrowed(drawn->Document, Version::R2018, &told);
  CHECK(std::ranges::none_of(lifted.Objects, Looking));
  REQUIRE_FALSE(told.empty());
  CHECK(told.front().What == "visual styles left out, R2018 holds them otherwise");
  CHECK(std::ranges::none_of(lifted.Vport.Records, Pointing, &Tables::Vport::Style));
  const Document same = Narrowed(drawn->Document, Version::R2007, nullptr);
  CHECK(std::ranges::any_of(same.Objects, Looking));
}

TEST_CASE("down: a header variable naming a visual style left out is cleared")
{
  const auto newer = Load(Bytes("kinds-r2018.dxf"));
  REQUIRE(newer);
  const Document down = Narrowed(newer->Document, Version::R2010, nullptr);
  const auto style = std::ranges::find(down.Header, "$INTERFEREOBJVS", &Header::Variable::Name);
  REQUIRE(style != down.Header.end());
  CHECK_FALSE(std::get<Handle>(style->Values.front().Value));
}

TEST_CASE("down: a viewport and a view lose a style that goes, keep one that stays")
{
  Document drawing = Create(Version::R2018);
  Objects::VisualStyle look{};
  look.Base.Self = Handle{.Value = 0x900};
  drawing.Objects.emplace_back(look);
  Entities::Viewport port{};
  port.Port->Style = Handle{.Value = 0x900};
  drawing.Blocks.front().Entities.emplace_back(port);
  Entities::Viewport other{};
  other.Port->Style = Handle{.Value = 0x901};
  drawing.Entities.emplace_back(other);
  drawing.Entities.emplace_back(Entities::Line{});
  Tables::View view{};
  view.Style = Handle{.Value = 0x900};
  drawing.View.Records.push_back(view);
  const Document down = Narrowed(drawing, Version::R2010, nullptr);
  CHECK_FALSE(std::get<Entities::Viewport>(down.Blocks.front().Entities.back()).Port->Style);
  CHECK(std::get<Entities::Viewport>(down.Entities.at(down.Entities.size() - 2)).Port->Style == Handle{.Value = 0x901});
  CHECK_FALSE(down.View.Records.back().Style);
}

TEST_CASE("down: no visual style to leave out, nothing told")
{
  const Document drawing = Create(Version::R2018);
  std::vector<Issue> told;
  static_cast<void>(Narrowed(drawing, Version::R2010, &told));
  CHECK(told.empty());
}

} // namespace DxfIO::Down

namespace DxfIO::Down
{
TEST_CASE("down: a visual style named by a hard pointer goes from that dictionary too")
{
  const auto drawn = Load(Bytes("kinds-r2018.dxf"));
  REQUIRE(drawn);
  Document drawing = drawn->Document;
  Handle look{};
  for (const Object &one : drawing.Objects)
  {
    if (const auto *held = std::get_if<Objects::VisualStyle>(&one))
    {
      look = held->Base.Self;
    }
  }
  REQUIRE(look);
  Objects::Dictionary hard{};
  hard.Base.Self = Past(drawing);
  Objects::Entry entry{};
  entry.Name = "Mine";
  entry.Hard = look;
  hard.Entries.push_back(entry);
  drawing.Objects.emplace_back(hard);
  const Document down = Narrowed(drawing, Version::R2010, nullptr);
  CHECK(std::get<Objects::Dictionary>(down.Objects.back()).Entries.empty());
}

TEST_CASE("down: R14 leaves out what a kept dictionary names but has no kind for")
{
  Document drawing = Create(Version::R2018);
  Handle group{};
  for (const Objects::Entry &one : std::get<Objects::Dictionary>(drawing.Objects.front()).Entries)
  {
    if (one.Name == "ACAD_GROUP")
    {
      group = one.Soft ? one.Soft : one.Hard;
    }
  }
  REQUIRE(group);
  Objects::Scale scale{};
  scale.Base.Self = Past(drawing);
  scale.Base.Owner = group;
  for (Object &one : drawing.Objects)
  {
    auto *held = std::get_if<Objects::Dictionary>(&one);
    if (held != nullptr && held->Base.Self == group)
    {
      Objects::Entry entry{};
      entry.Name = "Scaled";
      entry.Soft = scale.Base.Self;
      held->Entries.push_back(entry);
    }
  }
  drawing.Objects.emplace_back(scale);
  const Document down = Narrowed(drawing, Version::R14, nullptr);
  CHECK(std::ranges::none_of(down.Objects,
                             [](const Object &one) -> bool { return std::holds_alternative<Objects::Scale>(one); }));
}

TEST_CASE("down: R14 with no root first keeps an image's reactor as it is")
{
  Document drawing = Create(Version::R2018);
  drawing.Objects = {Object{Objects::ImageDefReactor{}}};
  const Document down = Narrowed(drawing, Version::R14, nullptr);
  REQUIRE(down.Objects.size() == 1);
  CHECK(std::holds_alternative<Objects::ImageDefReactor>(down.Objects.front()));
}
} // namespace DxfIO::Down
