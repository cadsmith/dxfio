#include "dxfio/audit/audit.hpp"

#include "../disk.hpp"
#include "../written.hpp"
#include "entities/io.hpp"
#include "handseed.hpp"
#include "past.hpp"

#include "dxfio/create.hpp"
#include "dxfio/edit/add.hpp"
#include "dxfio/edit/find.hpp"
#include "dxfio/load.hpp"
#include "dxfio/save.hpp"

#include <doctest/doctest.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <format>
#include <limits>
#include <numbers>
#include <string>
#include <string_view>
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
      out.Record.Base.Self  = self;
      out.Record.Base.Owner = Handle{.Value = 0x1f};
      out.Record.Layer      = Symbol{layer};
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
      const std::string text  = Bytes(path);
      const auto        drawn = Load(text);
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
    Document     drawing = Fresh();
    const Handle held    = drawing.Layer.Records.front().Record.Base.Self;
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
    Document           drawing = Fresh();
    Entities::Polyline heavy{};
    heavy.Record.Base.Owner = Handle{.Value = 0x1f};
    Entities::Vertex point{};
    point.Record.Base.Self  = Handle{.Value = 0x500};
    point.Record.Base.Owner = Handle{};
    heavy.Vertices.Children.push_back(point);
    heavy.Vertices.Closing =
        Boxed{Entities::Seqend{.Record = {.Base = {.Self = Handle{.Value = 0x501}}}}};
    drawing.Entities.emplace_back(heavy);
    WriteSeed(drawing, Past(drawing));
    const Report fixed = Repair(drawing);
    REQUIRE(fixed.Errors()            == 1);
    CHECK(fixed.Findings.front().What == "a POLYLINE has no handle");
    const auto &made = std::get<Entities::Polyline>(drawing.Entities.back());
    CHECK(made.Record.Base.Self);
    CHECK(made.Vertices.Children.front().Record.Base.Owner == made.Record.Base.Self);
  }

  TEST_CASE("audit: a vertex sharing a handle moves, not what it shares with")
  {
    Document           drawing = Fresh();
    const Handle       held    = drawing.Layer.Records.front().Record.Base.Self;
    Entities::Polyline heavy{};
    heavy.Record.Base.Self = Handle{.Value = 0x600};
    Entities::Vertex point{};
    point.Record.Base.Self = held;
    heavy.Vertices.Children.push_back(point);
    heavy.Vertices.Closing =
        Boxed{Entities::Seqend{.Record = {.Base = {.Self = Handle{.Value = 0x601}}}}};
    drawing.Entities.emplace_back(heavy);
    WriteSeed(drawing, Past(drawing));
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

  TEST_CASE(
      "audit: extended data of an application not registered, wherever it hangs, its APPID made")
  {
    Document   drawing = Fresh();
    const auto named   = [](std::string_view application) -> Xdata
    {
      return Xdata{.Application = std::string{application}};
    };
    Entities::Polyline polyline{};
    polyline.Record.Base.Extended.push_back(named("SOUTH"));
    polyline.Record.Base.Extended.push_back(named("acad"));
    polyline.Vertices.Children.push_back(Entities::Vertex{});
    polyline.Vertices.Children.front().Record.Base.Extended.push_back(named("EAST"));
    REQUIRE(Edit::Add(drawing, polyline));
    Tables::Layer layer{};
    layer.Record.Name = "WALLS";
    layer.Record.Base.Extended.push_back(named("NORTH"));
    REQUIRE(Edit::Add(drawing, layer));
    REQUIRE(Edit::AddBlock(drawing, "PART"));
    Edit::FindBlock(drawing, "PART")->Begin.Record.Base.Extended.push_back(named("WEST"));
    std::get<Objects::Dictionary>(drawing.Objects.front()).Base.Extended.push_back(named("south"));
    Tables::Table<Unknown> loose{};
    loose.Head.Name = "VX_TABLE";
    loose.Records.push_back(Unknown{.Type = Symbol{"VX_TABLE"}});
    loose.Records.front().Base.Extended.push_back(named("LOOSE"));
    drawing.Loose.push_back(loose);
    const Report seen = Check(drawing);
    CHECK(seen.Errors() == 5);
    CHECK(seen.Findings.front().What    == "application NORTH not registered");
    CHECK(seen.Findings.front().Subject == Edit::Find(drawing.Layer, "WALLS")->Record.Base.Self);
    const Report fixed = Repair(drawing);
    CHECK(fixed.Clean());
    for (const std::string_view application : {"SOUTH", "EAST", "NORTH", "WEST", "LOOSE"})
    {
      CAPTURE(application);
      const Tables::Appid *made = Edit::Find(drawing.Appid, application);
      REQUIRE(made != nullptr);
      CHECK(made->Record.Base.Self);
      CHECK(made->Record.Base.Owner == drawing.Appid.Head.Base.Self);
    }
    CHECK(drawing.Appid.Records.size() == 6);
    CHECK(Check(drawing).Findings.empty());
  }

  TEST_CASE("audit: R12 applications made carry no handle")
  {
    Document       drawing = Fresh(Version::R12);
    Entities::Line line{};
    line.Record.Base.Extended.push_back(Xdata{.Application = "SOUTH"});
    REQUIRE(Edit::Add(drawing, line));
    REQUIRE(Repair(drawing).Clean());
    const Tables::Appid *made = Edit::Find(drawing.Appid, "SOUTH");
    REQUIRE(made != nullptr);
    CHECK_FALSE(made->Record.Base.Self);
  }

  TEST_CASE("audit: an ellipse whose ratio is above 1 has its axes swapped, the curve the same")
  {
    Document          drawing = Fresh();
    Entities::Ellipse whole{};
    whole.Center              = Vector{.X = 1, .Y = 2, .Z = 0};
    whole.Axis                = Vector{.X = 3, .Y = 0, .Z = 0};
    whole.Ratio               = 2;
    Entities::Ellipse part    = whole;
    part.Start                = std::numbers::pi / 2;
    part.End                  = std::numbers::pi;
    Entities::Ellipse wrapped = whole;
    wrapped.Start             = std::numbers::pi / 4;
    wrapped.End               = 3 * std::numbers::pi / 4;
    Entities::Ellipse flat    = whole;
    flat.Axis                 = Vector{};
    REQUIRE(Edit::Add(drawing, whole));
    REQUIRE(Edit::Add(drawing, wrapped));
    REQUIRE(Edit::Add(drawing, flat));
    REQUIRE(Edit::AddBlock(drawing, "PART"));
    REQUIRE(Edit::Add(drawing, part, "PART"));
    const Report seen = Check(drawing);
    REQUIRE(seen.Errors()            == 4);
    CHECK(seen.Findings.front().What == "ellipse ratio 2 not between 0 and 1");
    CHECK(seen.Findings.front().Subject == BaseOf(drawing.Entities.front()).Self);
    CHECK(Repair(drawing).Clean());
    const auto &swapped = std::get<Entities::Ellipse>(drawing.Entities.front());
    CHECK(swapped.Axis  == Vector{.X = 0, .Y = 6, .Z = 0});
    CHECK(swapped.Ratio == 0.5);
    CHECK(swapped.Start == 0);
    CHECK(swapped.End   == Entities::Turn);
    const auto &turned = std::get<Entities::Ellipse>(drawing.Entities.at(1));
    CHECK(turned.Start == doctest::Approx(7 * std::numbers::pi / 4));
    CHECK(turned.End   == doctest::Approx(9 * std::numbers::pi / 4));
    const auto &line = std::get<Entities::Ellipse>(drawing.Entities.at(2));
    CHECK(line.Axis  == Vector{});
    CHECK(line.Ratio == 0.5);
    const auto &arc =
        std::get<Entities::Ellipse>(Edit::FindBlock(drawing, "PART")->Entities.front());
    CHECK(arc.Axis  == Vector{.X = 0, .Y = 6, .Z = 0});
    CHECK(arc.Start == doctest::Approx(0));
    CHECK(arc.End   == doctest::Approx(std::numbers::pi / 2));
    CHECK(Check(drawing).Findings.empty());
  }

  TEST_CASE(
      "audit: an ellipse whose ratio is 0 or less, or no number, takes the least one AutoCAD takes")
  {
    for (const double ratio : {0.0, -1.0, std::numeric_limits<double>::quiet_NaN()})
    {
      CAPTURE(ratio);
      Document          drawing = Fresh();
      Entities::Ellipse flat{};
      flat.Ratio = ratio;
      REQUIRE(Edit::Add(drawing, flat));
      CHECK(Check(drawing).Errors() == 1);
      CHECK(Repair(drawing).Clean());
      CHECK(std::get<Entities::Ellipse>(drawing.Entities.front()).Ratio == 1e-6);
      CHECK(Check(drawing).Findings.empty());
    }
    Document          drawing = Fresh();
    Entities::Ellipse round{};
    round.Ratio = 1;
    REQUIRE(Edit::Add(drawing, round));
    CHECK(Check(drawing).Findings.empty());
  }

  TEST_CASE(
      "audit: a kind the CLASSES section does not declare, its CLASS made as AutoCAD writes it")
  {
    Document       drawing = Fresh();
    Entities::Mesh mesh{};
    mesh.Net->Vertices = {Vector{}, Vector{.X = 1}, Vector{.Y = 1}};
    mesh.Net->Faces    = {3, 0, 1, 2};
    REQUIRE(Edit::Add(drawing, Entities::Wipeout{}));
    REQUIRE(Edit::Add(drawing, mesh));
    REQUIRE(Edit::Add(drawing, mesh));
    REQUIRE(Edit::Add(drawing, Entities::Line{}));
    REQUIRE(Edit::AddBlock(drawing, "PART"));
    REQUIRE(Edit::Add(drawing, Entities::Helix{}, "PART"));
    Objects::DictionaryVar variable{};
    variable.Base.Self = Handle{.Value = Past(drawing).Value + 1};
    drawing.Objects.push_back(variable);
    WriteSeed(drawing, Handle{.Value = Past(drawing).Value + 1});
    drawing.Classes.push_back(
        Raw{.Type = "CLASS", .Groups = {Group{.Code = 2, .Value = std::string{"AcDbThing"}}}});
    drawing.Classes.push_back(
        Raw{.Type = "CLASS", .Groups = {Group{.Code = 1, .Value = std::int64_t{5}}}});
    const Report seen = Check(drawing);
    REQUIRE(seen.Errors()          == 4);
    CHECK(seen.Findings.at(0).What == "no CLASS for WIPEOUT");
    CHECK(seen.Findings.at(1).What == "no CLASS for MESH");
    CHECK(seen.Findings.at(2).What == "no CLASS for HELIX");
    CHECK(seen.Findings.at(3).What == "no CLASS for DICTIONARYVAR");
    const std::size_t before = drawing.Classes.size();
    CHECK(Repair(drawing).Clean());
    REQUIRE(drawing.Classes.size() == before + 4);
    CHECK(
        drawing.Classes.at(before)
        == Raw{
            .Type   = "CLASS",
            .Groups = {
                Group{.Code = 1, .Value = std::string{"WIPEOUT"}},
                Group{.Code = 2, .Value = std::string{"AcDbWipeout"}},
                Group{.Code = 3,
                      .Value =
                          std::string{
                              "WipeOut|Product Desc: Object Enabler for WipeOut entity | Company: "
                              "Autodesk, Inc. | WEB Address: www.autodesk.com"}},
                Group{.Code = 90, .Value = std::int64_t{2175}},
                Group{.Code = 91, .Value = std::int64_t{1}},
                Group{.Code = 280, .Value = std::int64_t{0}},
                Group{.Code = 281, .Value = std::int64_t{1}},
            }});
    CHECK(drawing.Classes.at(before + 1).Groups.at(4)
          == Group{.Code = 91, .Value = std::int64_t{2}});
    CHECK(drawing.Classes.at(before + 3).Groups.at(6)
          == Group{.Code = 281, .Value = std::int64_t{0}});
    CHECK(Check(drawing).Findings.empty());
    const auto saved = Tests::Written(drawing);
    REQUIRE(saved);
    const auto back = Load(*saved);
    REQUIRE(back);
    CHECK(back->Issues.empty());
    CHECK(back->Document.Classes.size() == drawing.Classes.size());
  }

  TEST_CASE(
      "audit: a kind's CLASS is asked for in the releases AutoCAD writes it, and before R13 never")
  {
    Document early = Fresh(Version::R2000);
    std::erase_if(
        early.Classes, [](const Raw &one) -> bool
        { return one.Groups.front() == Group{.Code = 1, .Value = std::string{"LAYOUT"}}; });
    const Report seen = Check(early);
    REQUIRE(seen.Errors()            == 1);
    CHECK(seen.Findings.front().What == "no CLASS for LAYOUT");
    CHECK(Repair(early).Clean());
    CHECK(early.Classes.back().Groups.size() == 6);
    Objects::DataTable later{};
    later.Base.Self = Handle{.Value = Past(early).Value + 1};
    early.Objects.push_back(later);
    WriteSeed(early, Handle{.Value = Past(early).Value + 1});
    CHECK(Check(early).Findings.empty());
    CHECK(Check(Fresh(Version::R2004)).Findings.empty());
    Document old = Fresh(Version::R12);
    REQUIRE(Edit::Add(old, Entities::Line{}));
    CHECK(Check(old).Findings.empty());
  }

  TEST_CASE("audit: a dimension style named nowhere becomes the standard one, on every kind that "
            "names one")
  {
    Document            drawing = Fresh();
    Entities::Dimension dimension{};
    dimension.Figure->Measure.Style = Symbol{"NOPE"};
    Entities::ArcDimension arc{};
    arc.Bend->Measure.Style = Symbol{"NOPE"};
    Entities::LargeRadialDimension jogged{};
    jogged.Jog->Measure.Style = Symbol{"NOPE"};
    Entities::Leader leader{};
    leader.Lead->Style  = Symbol{"NOPE"};
    leader.Lead->Points = {Vector{}, Vector{.X = 1}};
    Entities::Tolerance tolerance{};
    tolerance.Style = Symbol{"NOPE"};
    Entities::Dimension fine{};
    fine.Figure->Measure.Style = Symbol{"standard"};
    REQUIRE(Edit::Add(drawing, dimension));
    REQUIRE(Edit::Add(drawing, arc));
    REQUIRE(Edit::Add(drawing, jogged));
    REQUIRE(Edit::Add(drawing, leader));
    REQUIRE(Edit::Add(drawing, tolerance));
    REQUIRE(Edit::Add(drawing, fine));
    const Report seen = Check(drawing);
    CHECK(seen.Warnings()            == 5);
    CHECK(seen.Findings.front().What == "dimension style NOPE missing");
    CHECK(seen.Findings.front().Subject == BaseOf(drawing.Entities.front()).Self);
    REQUIRE(Repair(drawing).Warnings()  == 5);
    CHECK(std::get<Entities::Dimension>(drawing.Entities.at(0)).Figure->Measure.Style.View()
          == "Standard");
    CHECK(std::get<Entities::ArcDimension>(drawing.Entities.at(1)).Bend->Measure.Style.View()
          == "Standard");
    CHECK(std::get<Entities::LargeRadialDimension>(drawing.Entities.at(2)).Jog->Measure.Style.View()
          == "Standard");
    CHECK(std::get<Entities::Leader>(drawing.Entities.at(3)).Lead->Style.View() == "Standard");
    CHECK(std::get<Entities::Tolerance>(drawing.Entities.at(4)).Style.View()    == "Standard");
    CHECK(std::get<Entities::Dimension>(drawing.Entities.at(5)).Figure->Measure.Style.View()
          == "standard");
    CHECK(Check(drawing).Findings.empty());
  }

  TEST_CASE("audit: a record or a block with no name is left out, what named it falls back")
  {
    Document      drawing = Fresh();
    Tables::Layer nameless{};
    nameless.Record.Base.Self  = Handle{.Value = Past(drawing).Value + 1};
    nameless.Record.Base.Owner = drawing.Layer.Head.Base.Self;
    drawing.Layer.Records.push_back(nameless);
    Tables::Ltype unlined{};
    unlined.Record.Base.Self = Handle{.Value = Past(drawing).Value + 1};
    drawing.Ltype.Records.push_back(unlined);
    Tables::Style shape{};
    shape.Record.Flags     = 1;
    shape.Record.Base.Self = Handle{.Value = Past(drawing).Value + 1};
    drawing.Style.Records.push_back(shape);
    Tables::Style unstyled{};
    unstyled.Record.Base.Self = Handle{.Value = Past(drawing).Value + 1};
    drawing.Style.Records.push_back(unstyled);
    WriteSeed(drawing, Handle{.Value = Past(drawing).Value + 1});
    REQUIRE(Edit::AddBlock(drawing, "PART"));
    Edit::FindBlock(drawing, "PART")->Begin.Name.clear();
    Edit::Find(drawing.BlockRecord, "PART")->Record.Name.clear();
    Entities::Line line{};
    line.Record.Layer    = Symbol{""};
    line.Record.Linetype = Symbol{""};
    REQUIRE(Edit::Add(drawing, line));
    Entities::Text text{};
    text.Lettering.Style = Symbol{""};
    REQUIRE(Edit::Add(drawing, text));
    Entities::Leader leader{};
    leader.Lead->Points = {Vector{}, Vector{.X = 1}};
    REQUIRE(Edit::Add(drawing, leader));
    Entities::Insert insert{};
    REQUIRE(Edit::Add(drawing, insert));
    const Report seen = Check(drawing);
    CHECK(seen.Errors()            == 7);
    CHECK(seen.Warnings()          == 3);
    CHECK(seen.Findings.at(0).What == "linetype with no name, left out");
    CHECK(seen.Findings.at(1).What == "layer with no name, left out");
    CHECK(seen.Findings.at(2).What == "text style with no name, left out");
    CHECK(seen.Findings.at(3).What == "block record with no name, left out");
    CHECK(seen.Findings.at(4).What == "block with no name, left out");
    CHECK(seen.Findings.at(5).What == "layer with no name, 0 taken");
    CHECK(seen.Findings.at(6).What == "linetype with no name");
    CHECK(seen.Findings.at(7).What == "text style with no name");
    CHECK(seen.Findings.at(8).What == "dimension style with no name");
    CHECK(seen.Findings.at(9).What == "block with no name, its insert left out");
    CHECK(Repair(drawing).Clean());
    CHECK(std::ranges::none_of(drawing.Layer.Records, [](const Tables::Layer &one) -> bool
                               { return one.Record.Name.empty(); }));
    CHECK(std::ranges::none_of(drawing.Ltype.Records, [](const Tables::Ltype &one) -> bool
                               { return one.Record.Name.empty(); }));
    CHECK(std::ranges::count_if(drawing.Style.Records, [](const Tables::Style &one) -> bool
                                { return one.Record.Name.empty(); })
          == 1);
    CHECK(drawing.BlockRecord.Records.size() == 2);
    CHECK(drawing.Blocks.size()     == 2);
    REQUIRE(drawing.Entities.size() == 3);
    const auto &lined = std::get<Entities::Line>(drawing.Entities.at(0));
    CHECK(lined.Record.Layer.View()    == "0");
    CHECK(lined.Record.Linetype.View() == "ByLayer");
    CHECK(std::get<Entities::Text>(drawing.Entities.at(1)).Lettering.Style.View() == "Standard");
    CHECK(std::get<Entities::Leader>(drawing.Entities.at(2)).Lead->Style.View()   == "Standard");
    CHECK(Check(drawing).Findings.empty());
  }

  TEST_CASE("audit: a leader with fewer than two vertices is left out")
  {
    Document         drawing = Fresh();
    Entities::Leader none{};
    none.Lead->Style     = Symbol{"Standard"};
    Entities::Leader one = none;
    one.Lead->Points     = {Vector{}};
    Entities::Leader two = none;
    two.Lead->Points     = {Vector{}, Vector{.X = 1}};
    REQUIRE(Edit::Add(drawing, none));
    REQUIRE(Edit::Add(drawing, two));
    REQUIRE(Edit::Add(drawing, one));
    REQUIRE(Edit::AddBlock(drawing, "PART"));
    REQUIRE(Edit::Add(drawing, one, "PART"));
    const Report seen = Check(drawing);
    REQUIRE(seen.Errors()            == 3);
    CHECK(seen.Findings.front().What == "leader with fewer than two vertices, left out");
    CHECK(seen.Findings.front().Subject == BaseOf(drawing.Entities.front()).Self);
    CHECK(seen.Warnings() == 0);
    CHECK(Repair(drawing).Clean());
    REQUIRE(drawing.Entities.size() == 1);
    CHECK(std::get<Entities::Leader>(drawing.Entities.front()).Lead->Points.size() == 2);
    CHECK(Edit::FindBlock(drawing, "PART")->Entities.empty());
    CHECK(Check(drawing).Findings.empty());
  }

  TEST_CASE("audit: a vertex flagged for another kind of polyline than its own is reflagged")
  {
    Document           drawing = Fresh();
    Entities::Polyline spatial{};
    spatial.Flags             = 8;
    spatial.Vertices.Children = {Entities::Vertex{}, Entities::Vertex{}};
    REQUIRE(Edit::Add(drawing, spatial));
    auto &vertices = std::get<Entities::Polyline>(drawing.Entities.front()).Vertices.Children;
    vertices.front().Flags = 0;
    vertices.back().Flags  = 64 + 1;
    const Report seen      = Check(drawing);
    REQUIRE(seen.Errors()            == 2);
    CHECK(seen.Findings.front().What == "vertex flagged 0 where its polyline asks 32");
    CHECK(seen.Findings.front().Subject == vertices.front().Record.Base.Self);
    CHECK(seen.Findings.back().What     == "vertex flagged 65 where its polyline asks 33");
    CHECK(Repair(drawing).Clean());
    CHECK(vertices.front().Flags == 32);
    CHECK(vertices.back().Flags  == 33);
    CHECK(Check(drawing).Findings.empty());
  }

  TEST_CASE("audit: a block holding attribute definitions and not flagged so is flagged")
  {
    Document drawing = Fresh();
    REQUIRE(Edit::AddBlock(drawing, "TAGGED"));
    REQUIRE(Edit::Add(drawing, Entities::Attdef{}, "TAGGED"));
    REQUIRE(Edit::AddBlock(drawing, "FIXED"));
    Entities::Attdef constant{};
    constant.Flags = 2;
    REQUIRE(Edit::Add(drawing, constant, "FIXED"));
    Blocks::Block *tagged = Edit::FindBlock(drawing, "TAGGED");
    tagged->Begin.Flags   = 1;
    const Report seen     = Check(drawing);
    REQUIRE(seen.Warnings() == 1);
    CHECK(seen.Findings.front().What == "block TAGGED holds attribute definitions, not flagged so");
    CHECK(seen.Findings.front().Subject == tagged->Begin.Record.Base.Self);
    CHECK(seen.Errors() == 0);
    CHECK(Repair(drawing).Clean());
    CHECK(tagged->Begin.Flags == 3);
    CHECK(Edit::FindBlock(drawing, "FIXED")->Begin.Flags == 0);
    CHECK(Check(drawing).Findings.empty());
  }

  TEST_CASE("audit: a group of a code DXF does not define is left out, wherever it is kept")
  {
    Document       drawing = Fresh();
    Entities::Line line{};
    line.Record.Base.Extra.push_back(Group{.Code = 250, .Value = std::string{"2"}});
    line.Record.Base.Extra.push_back(Group{.Code = 300, .Value = std::string{"kept"}});
    line.Record.Base.Extended.push_back(
        Xdata{.Application = "ACAD",
              .Groups      = {Group{.Code = 1080, .Value = std::string{"x"}},
                              Group{.Code = 1000, .Value = std::string{"kept"}}}});
    REQUIRE(Edit::Add(drawing, line));
    Unknown loose{};
    loose.Type   = Symbol{"CAMERA"};
    loose.Groups = {Group{.Code = 83, .Value = std::string{"5"}},
                    Group{.Code = 100, .Value = std::string{"AcDbCamera"}}};
    REQUIRE(Edit::Add(drawing, loose));
    const Report seen = Check(drawing);
    REQUIRE(seen.Errors()             == 3);
    CHECK(seen.Findings.at(0).What    == "group 250 is no DXF group, left out");
    CHECK(seen.Findings.at(0).Subject == BaseOf(drawing.Entities.front()).Self);
    CHECK(seen.Findings.at(1).What    == "group 1080 is no DXF group, left out");
    CHECK(seen.Findings.at(2).What    == "group 83 is no DXF group, left out");
    CHECK(Repair(drawing).Clean());
    const Base &base = BaseOf(drawing.Entities.front());
    CHECK(base.Extra == std::vector{Group{.Code = 300, .Value = std::string{"kept"}}});
    CHECK(base.Extended.front().Groups
          == std::vector{Group{.Code = 1000, .Value = std::string{"kept"}}});
    CHECK(std::get<Unknown>(drawing.Entities.back()).Groups.size() == 1);
    CHECK(Check(drawing).Findings.empty());
  }

  TEST_CASE("audit: a $HANDSEED behind the handles in use, or none, is an error that Repair moves "
            "past them")
  {
    Document drawing = Fresh();
    REQUIRE(Edit::Add(drawing, Entities::Line{}));
    BaseOf(drawing.Entities.front()).Self = Handle{.Value = 0xABCDE};
    const Report seen                     = Check(drawing);
    REQUIRE(seen.Errors() == 1);
    CHECK(seen.Findings.front().What
          == std::format("$HANDSEED {} behind the handles in use", Hex(Seed(drawing))));
    CHECK(Repair(drawing).Clean());
    CHECK(Seed(drawing) == Handle{.Value = 0xABCDF});
    CHECK(Check(drawing).Findings.empty());
    std::erase_if(drawing.Header,
                  [](const Header::Variable &one) -> bool { return one.Name == "$HANDSEED"; });
    const Report missing = Check(drawing);
    REQUIRE(missing.Errors()            == 1);
    CHECK(missing.Findings.front().What == "no $HANDSEED");
    CHECK(Repair(drawing).Clean());
    CHECK(Check(drawing).Findings.empty());
    Document bare{};
    bare.Entities.push_back(Entities::Line{});
    CHECK(std::ranges::none_of(Check(bare).Findings, [](const Finding &one) -> bool
                               { return one.What.contains("$HANDSEED"); }));
  }

  TEST_CASE(
      "audit: a lineweight AutoCAD does not have takes the nearest it has, on entities and layers")
  {
    Document       drawing = Fresh();
    Entities::Line heavy{};
    heavy.Record.Lineweight = 284;
    Entities::Line odd{};
    odd.Record.Lineweight = -4;
    Entities::Polyline chain{};
    chain.Vertices.Children.push_back(Entities::Vertex{});
    chain.Vertices.Children.front().Record.Lineweight = 7;
    Entities::Line fine{};
    fine.Record.Lineweight = 211;
    REQUIRE(Edit::Add(drawing, heavy));
    REQUIRE(Edit::Add(drawing, odd));
    REQUIRE(Edit::Add(drawing, chain));
    REQUIRE(Edit::Add(drawing, fine));
    Tables::Layer walls{};
    walls.Record.Name = "WALLS";
    walls.Lineweight  = 1000;
    REQUIRE(Edit::Add(drawing, walls));
    const Report seen = Check(drawing);
    CHECK(seen.Errors()            == 0);
    REQUIRE(seen.Warnings()        == 4);
    CHECK(seen.Findings.at(0).What == "lineweight 1000 is none AutoCAD has, 211 taken");
    CHECK(seen.Findings.at(0).Subject == Edit::Find(drawing.Layer, "WALLS")->Record.Base.Self);
    CHECK(seen.Findings.at(1).What    == "lineweight 284 is none AutoCAD has, 211 taken");
    CHECK(seen.Findings.at(2).What    == "lineweight -4 is none AutoCAD has, -3 taken");
    CHECK(seen.Findings.at(3).What    == "lineweight 7 is none AutoCAD has, 5 taken");
    CHECK(Repair(drawing).Clean());
    CHECK(Edit::Find(drawing.Layer, "WALLS")->Lineweight == 211);
    CHECK(std::get<Entities::Line>(drawing.Entities.at(0)).Record.Lineweight == 211);
    CHECK(std::get<Entities::Line>(drawing.Entities.at(1)).Record.Lineweight == -3);
    CHECK(std::get<Entities::Polyline>(drawing.Entities.at(2))
              .Vertices.Children.front()
              .Record.Lineweight
          == 5);
    CHECK(std::get<Entities::Line>(drawing.Entities.at(3)).Record.Lineweight == 211);
    CHECK(Check(drawing).Findings.empty());
  }

  TEST_CASE("audit: the records AutoCAD needs, put back with their defaults")
  {
    Document drawing = Fresh();
    std::erase_if(drawing.Layer.Records,
                  [](const Tables::Layer &one) -> bool { return one.Record.Name == "0"; });
    std::erase_if(drawing.Ltype.Records,
                  [](const Tables::Ltype &one) -> bool { return one.Record.Name == "Continuous"; });
    drawing.Appid.Records.clear();
    const Report seen = Check(drawing);
    CHECK(seen.Errors() == 3);
    CHECK(seen.Findings.front().What == "0 missing");
    const Report fixed = Repair(drawing);
    CHECK(fixed.Clean());
    CHECK(std::ranges::any_of(drawing.Layer.Records, [](const Tables::Layer &one) -> bool
                              { return one.Record.Name == "0"; }));
    CHECK(drawing.Appid.Records.size() == 1);
    CHECK(drawing.Appid.Records.front().Record.Base.Owner == drawing.Appid.Head.Base.Self);
    CHECK(Check(drawing).Findings.empty());
  }

  TEST_CASE("audit: a record put back names this drawing's own, not another's")
  {
    const auto drawn = Load(Bytes(Acad() / "entities-r2018.dxf"));
    REQUIRE(drawn);
    Document            drawing = drawn->Document;
    const Tables::Layer zero =
        *std::ranges::find_if(drawing.Layer.Records, [](const Tables::Layer &one) -> bool
                              { return one.Record.Name == "0"; });
    std::erase_if(drawing.Layer.Records,
                  [](const Tables::Layer &one) -> bool { return one.Record.Name == "0"; });
    std::erase_if(drawing.Dimstyle.Records, [](const Tables::Dimstyle &one) -> bool
                  { return one.Record.Name == "Standard"; });
    std::erase_if(drawing.Ltype.Records,
                  [](const Tables::Ltype &one) -> bool { return one.Record.Name == "ByBlock"; });
    std::erase_if(drawing.Vport.Records,
                  [](const Tables::Vport &one) -> bool { return one.Record.Name == "*Active"; });
    REQUIRE(Repair(drawing).Clean());
    const auto made =
        std::ranges::find_if(drawing.Layer.Records, [](const Tables::Layer &one) -> bool
                             { return one.Record.Name == "0"; });
    REQUIRE(made != drawing.Layer.Records.end());
    CHECK(made->Material == zero.Material);
    CHECK(made->Style    == zero.Style);
    const Tables::Dimstyle &style = drawing.Dimstyle.Records.front();
    CHECK(style.Record.Name == "Standard");
    CHECK(style.Dimtxsty    == drawing.Style.Records.front().Record.Base.Self);
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
    CHECK(drawing.Dimstyle.Records.front().Dimtxsty
          == drawing.Style.Records.front().Record.Base.Self);
  }

  TEST_CASE("audit: records put back where AutoCAD looks for them, in order")
  {
    const auto drawn = Load(Bytes(Acad() / "entities-r2018.dxf"));
    REQUIRE(drawn);
    Document drawing = drawn->Document;
    REQUIRE(drawing.Appid.Records.size() > 1);
    std::erase_if(drawing.Appid.Records,
                  [](const Tables::Appid &one) -> bool { return one.Record.Name == "ACAD"; });
    std::erase_if(drawing.Ltype.Records,
                  [](const Tables::Ltype &one) -> bool { return one.Record.Name == "Continuous"; });
    REQUIRE(Repair(drawing).Clean());
    CHECK(drawing.Appid.Records.front().Record.Name == "ACAD");
    CHECK(drawing.Ltype.Records.at(2).Record.Name   == "Continuous");
  }

  TEST_CASE("audit: R12 records put back carry no handle")
  {
    Document drawing = Fresh(Version::R12);
    drawing.Style.Records.clear();
    REQUIRE(Repair(drawing).Errors()     == 1);
    REQUIRE(drawing.Style.Records.size() == 1);
    CHECK_FALSE(drawing.Style.Records.front().Record.Base.Self);
  }

  TEST_CASE("audit: a layer named nowhere is made, its case aside")
  {
    Document drawing = Fresh();
    drawing.Entities.emplace_back(Lined(Past(drawing), "Walls"));
    drawing.Entities.emplace_back(Lined(Past(drawing), "WALLS"));
    WriteSeed(drawing, Past(drawing));
    CHECK(Check(drawing).Errors() == 2);
    const Report fixed = Repair(drawing);
    CHECK(fixed.Errors() == 1);
    CHECK(std::ranges::count_if(drawing.Layer.Records, [](const Tables::Layer &one) -> bool
                                { return one.Record.Name == "Walls"; })
          == 1);
    CHECK(Check(drawing).Findings.empty());
  }

  TEST_CASE("audit: a layer made from layer 0, its material and plot style")
  {
    Document drawing = Fresh();
    drawing.Entities.emplace_back(Lined(Past(drawing), "Walls"));
    WriteSeed(drawing, Past(drawing));
    REQUIRE(Repair(drawing).Errors() == 1);
    const Tables::Layer &made = drawing.Layer.Records.back();
    CHECK(made.Record.Name == "Walls");
    CHECK(made.Material    == drawing.Layer.Records.front().Material);
  }

  TEST_CASE("audit: a layer made from layer 0, wherever layer 0 stands")
  {
    Document      drawing  = Fresh();
    Tables::Layer walls    = drawing.Layer.Records.front();
    walls.Record.Name      = "Walls";
    walls.Material         = Handle{.Value = 0x77};
    walls.Record.Base.Self = Past(drawing);
    drawing.Layer.Records.insert(drawing.Layer.Records.begin(), walls);
    drawing.Entities.emplace_back(Lined(Past(drawing), "Doors"));
    WriteSeed(drawing, Past(drawing));
    REQUIRE(Repair(drawing).Errors() == 1);
    const Tables::Layer &made = drawing.Layer.Records.back();
    CHECK(made.Record.Name == "Doors");
    CHECK(made.Material    == drawing.Layer.Records.at(1).Material);
    CHECK(made.Material != walls.Material);
  }

  TEST_CASE("audit: a layer with no plot style takes the drawing's default one")
  {
    Document     drawing = Fresh();
    const Handle style   = drawing.Layer.Records.front().Style;
    REQUIRE(style);
    drawing.Layer.Records.front().Style = {};
    const Report found                  = Check(drawing);
    REQUIRE(found.Findings.size()     == 1);
    CHECK(found.Findings.front().What == "layer 0 has no plot style");
    CHECK_FALSE(drawing.Layer.Records.front().Style);
    CHECK(Repair(drawing).Errors()            == 1);
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
    Document       drawing = Fresh();
    Entities::Line one     = Lined(Past(drawing));
    one.Record.Linetype    = Symbol{"Dashed"};
    drawing.Entities.emplace_back(one);
    Entities::Line two  = Lined(Past(drawing));
    two.Record.Linetype = Symbol{"BYBLOCK"};
    drawing.Entities.emplace_back(two);
    WriteSeed(drawing, Past(drawing));
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
    Document       drawing = Fresh();
    Entities::Text text{};
    text.Record.Base.Self = Past(drawing);
    text.Lettering.Style  = Symbol{"Gone"};
    drawing.Entities.emplace_back(text);
    Entities::MText para{};
    para.Record.Base.Self = Handle{.Value = Past(drawing).Value};
    para.Paragraph->Style = Symbol{"Gone"};
    drawing.Entities.emplace_back(para);
    Entities::Text kept{};
    kept.Record.Base.Self = Handle{.Value = Past(drawing).Value};
    kept.Lettering.Style  = Symbol{"STANDARD"};
    drawing.Entities.insert(drawing.Entities.begin(), kept);
    WriteSeed(drawing, Past(drawing));
    REQUIRE(Repair(drawing).Warnings() == 2);
    CHECK(std::get<Entities::Text>(drawing.Entities.front()).Lettering.Style == "STANDARD");
    CHECK(std::get<Entities::Text>(drawing.Entities.at(drawing.Entities.size() - 2)).Lettering.Style
          == "Standard");
    CHECK(std::get<Entities::MText>(drawing.Entities.back()).Paragraph->Style == "Standard");
  }

  TEST_CASE("audit: an attribute definition's text style named nowhere becomes the standard one")
  {
    Document         drawing = Fresh();
    Entities::Attdef size{};
    size.Record.Base.Self = Past(drawing);
    size.Lettering->Style = Symbol{"Gone"};
    drawing.Entities.emplace_back(size);
    WriteSeed(drawing, Past(drawing));
    CHECK(Check(drawing).Warnings()    == 2);
    REQUIRE(Repair(drawing).Warnings() == 2);
    CHECK(std::get<Entities::Attdef>(drawing.Entities.back()).Lettering->Style == "Standard");
    CHECK(Edit::FindBlock(drawing, "*Model_Space")->Begin.Flags == 2);
  }

  TEST_CASE("audit: an insert of no block is left out")
  {
    Document         drawing = Fresh();
    Entities::Insert gone{};
    gone.Record.Base.Self = Past(drawing);
    gone.Block            = Symbol{"Nowhere"};
    drawing.Entities.emplace_back(gone);
    Entities::Insert kept{};
    kept.Record.Base.Self = Handle{.Value = gone.Record.Base.Self.Value + 1};
    kept.Block            = Symbol{"*model_space"};
    drawing.Entities.emplace_back(kept);
    const std::size_t before = drawing.Entities.size();
    WriteSeed(drawing, Past(drawing));
    CHECK(Check(drawing).Errors()    == 1);
    CHECK(drawing.Entities.size()    == before);
    REQUIRE(Repair(drawing).Errors() == 1);
    CHECK(drawing.Entities.size()    == before - 1);
  }

  TEST_CASE("audit: a mesh with no vertices or no faces is left out")
  {
    Document       drawing = Fresh();
    Entities::Mesh faceless{};
    faceless.Net->Vertices = {Vector{}, Vector{.X = 1}, Vector{.Y = 1}};
    Entities::Mesh pointless{};
    pointless.Net->Faces = {3, 0, 1, 2};
    REQUIRE(Edit::Add(drawing, faceless));
    REQUIRE(Edit::Add(drawing, Entities::Line{}));
    REQUIRE(Edit::AddBlock(drawing, "PART"));
    REQUIRE(Edit::Add(drawing, pointless, "PART"));
    const Report seen = Check(drawing);
    REQUIRE(seen.Errors()             == 3);
    CHECK(seen.Findings.at(0).What    == "mesh with no faces, left out");
    CHECK(seen.Findings.at(0).Subject == BaseOf(drawing.Entities.front()).Self);
    CHECK(seen.Findings.at(1).What    == "mesh with no vertices, left out");
    CHECK(seen.Findings.at(2).What    == "no CLASS for MESH");
    CHECK(drawing.Entities.size()     == 2);
    CHECK(Repair(drawing).Clean());
    REQUIRE(drawing.Entities.size() == 1);
    CHECK(std::holds_alternative<Entities::Line>(drawing.Entities.front()));
    CHECK(Edit::FindBlock(drawing, "PART")->Entities.empty());
    CHECK(Check(drawing).Findings.empty());
  }

  TEST_CASE("audit: a mesh with a crease short is given one")
  {
    Document       drawing = Fresh();
    Entities::Mesh mesh{};
    mesh.Record.Base.Self = Past(drawing);
    mesh.Net->Vertices    = {Vector{}, Vector{.X = 1}, Vector{.Y = 1}};
    mesh.Net->Faces       = {3, 0, 1, 2};
    mesh.Net->Edges       = {0, 1, 1, 2};
    mesh.Net->Creases     = {0.5};
    drawing.Blocks.front().Entities.emplace_back(mesh);
    Entities::Mesh whole   = mesh;
    whole.Net->Creases     = {0.5, 0.0};
    whole.Record.Base.Self = Handle{.Value = mesh.Record.Base.Self.Value + 1};
    drawing.Blocks.front().Entities.emplace_back(whole);
    drawing.Entities.emplace_back(Lined(Handle{.Value = mesh.Record.Base.Self.Value + 2}));
    WriteSeed(drawing, Past(drawing));
    const Report seen = Check(drawing);
    REQUIRE(seen.Warnings()            == 1);
    CHECK(seen.Findings.front().What   == "mesh with 1 creases for 2 edges");
    REQUIRE(Repair(drawing).Warnings() == 1);
    const auto &fixed = std::get<Entities::Mesh>(
        drawing.Blocks.front().Entities.at(drawing.Blocks.front().Entities.size() - 2));
    CHECK(fixed.Net->Creases == std::vector{0.5, 0.0});
  }

  TEST_CASE("audit: a drawing with no seed to set still repairs, and is given one")
  {
    Document drawing = Fresh();
    std::erase_if(drawing.Header,
                  [](const Header::Variable &one) -> bool { return one.Name == "$HANDSEED"; });
    drawing.Appid.Records.clear();
    CHECK(Repair(drawing).Clean());
    CHECK(Seed(drawing) == Past(drawing));
    Document   empty = Fresh();
    const auto seed  = std::ranges::find(empty.Header, "$HANDSEED", &Header::Variable::Name);
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
    Document       drawing = Create(Version::R2018);
    Entities::Text text{};
    text.Record.Base.Self = Past(drawing);
    text.Lettering.Style  = Symbol{"Gone"};
    drawing.Entities.emplace_back(text);
    drawing.Entities.emplace_back(Entities::Polyline{});
    WriteSeed(drawing, Past(drawing));
    const Report seen = Check(drawing);
    CHECK(seen.Errors()   == 2);
    CHECK(seen.Warnings() == 1);
    CHECK_FALSE(std::get<Entities::Polyline>(drawing.Entities.back()).Vertices.Closing);
    CHECK(std::ranges::none_of(seen.Findings, &Finding::Fixed));
    CHECK(std::get<Entities::Text>(drawing.Entities.front()).Lettering.Style == "Gone");
  }

  TEST_CASE("audit: a vertex owned elsewhere keeps its owner when its polyline moves")
  {
    Document           drawing = Create(Version::R2018);
    const Handle       held    = drawing.Layer.Records.front().Record.Base.Self;
    const Handle       next    = Past(drawing);
    Entities::Polyline heavy{};
    heavy.Record.Base.Self = held;
    Entities::Vertex mine{};
    mine.Record.Base.Self  = next;
    mine.Record.Base.Owner = held;
    Entities::Vertex other{};
    other.Record.Base.Self  = Handle{.Value = next.Value + 1};
    other.Record.Base.Owner = Handle{.Value = 0x77};
    heavy.Vertices.Children = {mine, other};
    heavy.Vertices.Closing =
        Boxed{Entities::Seqend{.Record = {.Base = {.Self = Handle{.Value = next.Value + 2}}}}};
    drawing.Entities.emplace_back(heavy);
    WriteSeed(drawing, Past(drawing));
    REQUIRE(Repair(drawing).Errors() == 1);
    const auto &made = std::get<Entities::Polyline>(drawing.Entities.back());
    CHECK(made.Vertices.Children.front().Record.Base.Owner == made.Record.Base.Self);
    CHECK(made.Vertices.Children.back().Record.Base.Owner  == Handle{.Value = 0x77});
  }
} // namespace DxfIO::Audit

namespace DxfIO::Audit
{
  TEST_CASE("audit: with the last handle there is held, the lowest free ones are taken")
  {
    constexpr Handle last{.Value = std::numeric_limits<std::uint64_t>::max()};
    Document         drawing = Create(Version::R2018);
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
    constexpr std::uint64_t last    = std::numeric_limits<std::uint64_t>::max();
    Document                drawing = Create(Version::R2018);
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
    Blocks::Block &moved                                   = *Edit::FindBlock(drawing, "B");
    moved.Begin.Record.Base.Owner                          = shared;
    moved.End.Record.Base.Owner                            = shared;
    BaseOf(moved.Entities.front()).Owner                   = shared;
    const Report fixed                                     = Repair(drawing);
    REQUIRE(fixed.Errors() == 1);
    CHECK(fixed.Findings.front().What
          == std::format("handle {:X} of a block record used twice", shared.Value));
    const Handle now = Edit::Find(drawing.BlockRecord, "B")->Record.Base.Self;
    CHECK(now != shared);
    CHECK(moved.Begin.Record.Base.Owner        == now);
    CHECK(moved.End.Record.Base.Owner          == now);
    CHECK(BaseOf(moved.Entities.front()).Owner == now);
    const Blocks::Block &kept = *Edit::FindBlock(drawing, "A");
    CHECK(kept.Begin.Record.Base.Owner        == shared);
    CHECK(BaseOf(kept.Entities.front()).Owner == shared);
  }

  TEST_CASE(
      "audit: the paper space record given a new handle takes its entities and its layout along")
  {
    Document drawing = Create(Version::R2018);
    REQUIRE(Edit::Add(drawing, Entities::Line{}));
    REQUIRE(Edit::Add(drawing, Entities::Circle{}, Edit::Space::Paper));
    const Tables::BlockRecord &model      = *Edit::Find(drawing.BlockRecord, "*Model_Space");
    const Handle               shared     = model.Record.Base.Self;
    Tables::BlockRecord       &paper      = *Edit::Find(drawing.BlockRecord, "*Paper_Space");
    paper.Record.Base.Self                = shared;
    Blocks::Block &sheet                  = *Edit::FindBlock(drawing, "*Paper_Space");
    sheet.Begin.Record.Base.Owner         = shared;
    sheet.End.Record.Base.Owner           = shared;
    BaseOf(drawing.Entities.back()).Owner = shared;
    auto &laid = std::get<Objects::Layout>(*Edit::FindObject(drawing, paper.Layout));
    laid.Block = shared;
    REQUIRE(Repair(drawing).Errors() == 1);
    const Handle now = paper.Record.Base.Self;
    CHECK(now != shared);
    CHECK(sheet.Begin.Record.Base.Owner         == now);
    CHECK(sheet.End.Record.Base.Owner           == now);
    CHECK(BaseOf(drawing.Entities.back()).Owner == now);
    CHECK(laid.Block == now);
    CHECK(BaseOf(drawing.Entities.front()).Owner == shared);
    CHECK(Edit::FindBlock(drawing, "*Model_Space")->Begin.Record.Base.Owner         == shared);
    CHECK(std::get<Objects::Layout>(*Edit::FindObject(drawing, model.Layout)).Block == shared);
  }

  TEST_CASE("audit: the model space record given a new handle leaves paper space's entities")
  {
    Document drawing = Create(Version::R2018);
    REQUIRE(Edit::Add(drawing, Entities::Line{}));
    REQUIRE(Edit::Add(drawing, Entities::Circle{}, Edit::Space::Paper));
    Tables::BlockRecord &model             = *Edit::Find(drawing.BlockRecord, "*Model_Space");
    const Handle         shared            = drawing.Layer.Head.Base.Self;
    model.Record.Base.Self                 = shared;
    BaseOf(drawing.Entities.front()).Owner = shared;
    BaseOf(drawing.Entities.back()).Owner  = shared;
    REQUIRE(Repair(drawing).Errors() == 1);
    const Handle now = model.Record.Base.Self;
    CHECK(now != shared);
    CHECK(BaseOf(drawing.Entities.front()).Owner == now);
    CHECK(BaseOf(drawing.Entities.back()).Owner  == shared);
    CHECK(drawing.Layer.Head.Base.Self           == shared);
  }

  TEST_CASE("audit: a table given a new handle takes its records along, not another's")
  {
    Document     drawing         = Create(Version::R2018);
    const Handle shared          = drawing.Ltype.Head.Base.Self;
    drawing.Layer.Head.Base.Self = shared;
    for (Tables::Layer &one : drawing.Layer.Records)
    {
      one.Record.Base.Owner = shared;
    }
    Tables::BlockRecord &model = drawing.BlockRecord.Records.front();
    model.Record.Base.Self     = drawing.BlockRecord.Head.Base.Self;
    REQUIRE(Repair(drawing).Errors() == 2);
    const Handle now = drawing.Layer.Head.Base.Self;
    CHECK(now != shared);
    CHECK(std::ranges::all_of(drawing.Layer.Records, [now](const Tables::Layer &one) -> bool
                              { return one.Record.Base.Owner == now; }));
    CHECK(std::ranges::all_of(drawing.Ltype.Records, [shared](const Tables::Ltype &one) -> bool
                              { return one.Record.Base.Owner == shared; }));
    CHECK(drawing.BlockRecord.Records.back().Record.Base.Owner
          == drawing.BlockRecord.Head.Base.Self);
  }

  TEST_CASE("audit: what an entity carries is checked too, for its layer, linetype and text style")
  {
    Document drawing = Create(Version::R2018);
    REQUIRE(Edit::AddBlock(drawing, "Valve"));
    Entities::Attrib number{};
    number.Record.Layer    = Symbol{"NoSuchLayer"};
    number.Lettering.Style = Symbol{"NoSuchStyle"};
    Entities::Insert placed{};
    placed.Block   = Symbol{"Valve"};
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
    CHECK(seen.Errors()   == 2);
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

  TEST_CASE(
      "audit: what carries vertices or attributes but no SEQEND is given one, an insert's flagged")
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
    placed.Follows    = false;
    const Report seen = Check(drawing);
    REQUIRE(seen.Errors()          == 3);
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
    valve.Block   = Symbol{"Valve"};
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
