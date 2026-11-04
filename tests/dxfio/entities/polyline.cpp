#include "entities/shapes/polyline.hpp"

#include "../kind.hpp"
#include "../written.hpp"
#include "heavy.hpp"

#include "dxfio/save.hpp"

#include <algorithm>
#include <initializer_list>
#include <string>
#include <string_view>
#include <variant>

namespace DxfIO::Entities
{
namespace
{
constexpr std::string_view Head = "  0\r\nPOLYLINE\r\n  5\r\n2A\r\n330\r\n1F\r\n"
                                  "100\r\nAcDbEntity\r\n  8\r\n0\r\n"
                                  "100\r\nAcDb2dPolyline\r\n 66\r\n     1\r\n"
                                  " 10\r\n0.0\r\n 20\r\n0.0\r\n 30\r\n0.0\r\n";

constexpr std::string_view Corner = "  0\r\nVERTEX\r\n  5\r\n2B\r\n330\r\n2A\r\n100\r\nAcDbEntity\r\n"
                                    "  8\r\n0\r\n100\r\nAcDbVertex\r\n100\r\nAcDb2dVertex\r\n"
                                    " 10\r\n1.0\r\n 20\r\n2.0\r\n 30\r\n0.0\r\n";

constexpr std::string_view Other = "  0\r\nVERTEX\r\n  5\r\n2C\r\n330\r\n2A\r\n100\r\nAcDbEntity\r\n"
                                   "  8\r\n0\r\n100\r\nAcDbVertex\r\n100\r\nAcDb2dVertex\r\n"
                                   " 10\r\n3.0\r\n 20\r\n4.0\r\n 30\r\n0.0\r\n";

constexpr std::string_view Closing = "  0\r\nSEQEND\r\n  5\r\n2D\r\n330\r\n2A\r\n"
                                     "100\r\nAcDbEntity\r\n  8\r\n0\r\n";

constexpr std::string_view Stray = "  0\r\nLINE\r\n  5\r\n2E\r\n330\r\n1F\r\n100\r\nAcDbEntity\r\n  8\r\n0\r\n"
                                   "100\r\nAcDbLine\r\n 10\r\n0.0\r\n 20\r\n0.0\r\n 30\r\n0.0\r\n"
                                   " 11\r\n1.0\r\n 21\r\n1.0\r\n 31\r\n0.0\r\n";

auto Joined(std::initializer_list<std::string_view> parts) -> std::string
{
  std::string out;
  for (const std::string_view part : parts)
  {
    out += part;
  }
  return out;
}
} // namespace

TEST_CASE("polyline: an open one, its vertices and SEQEND its own")
{
  const DxfIO::Document file = Heavy();
  const Polyline &taken = Traced(file.Entities, 0x2ce);
  REQUIRE(taken.Vertices.Children.size() == 3);
  CHECK(taken.Vertices.Children.at(1).Position == Vector{.X = 10, .Y = 0, .Z = 0});
  CHECK(taken.Vertices.Children.front().Record.Base.Owner == Handle{0x2ce});
  REQUIRE(taken.Vertices.Closing);
  CHECK((*taken.Vertices.Closing)->Record.Base.Owner == Handle{0x2ce});
  CHECK(taken.Follows);
  CHECK(taken.Flags == 0);
}

TEST_CASE("polyline: closed, wide, tapered, raised, thick, on another plane")
{
  const DxfIO::Document file = Heavy();
  CHECK(Traced(file.Entities, 0x2d3).Flags == 1);
  CHECK(Traced(file.Entities, 0x2d8).Start == 2);
  CHECK(Traced(file.Entities, 0x2d8).End == 2);
  CHECK(Traced(file.Entities, 0x2e6).Position.Z == 5);
  CHECK(Traced(file.Entities, 0x2eb).Thickness == 3);
  CHECK(Traced(file.Entities, 0x2f0).Extrusion.Y == -1);
  CHECK(Traced(file.Entities, 0x2f5).Flags == 128);
}

TEST_CASE("polyline: a vertex's width none where the polyline's holds")
{
  const DxfIO::Document file = Heavy();
  const Polyline &taken = Traced(file.Entities, 0x2dd);
  REQUIRE(taken.Vertices.Children.size() == 3);
  CHECK(taken.Start == 1);
  CHECK(taken.End == 3);
  CHECK_FALSE(taken.Vertices.Children.at(0).Start);
  CHECK(taken.Vertices.Children.at(1).Start == 3.0);
  CHECK(taken.Vertices.Children.at(1).End == 0.0);
}

TEST_CASE("polyline: fitted and splined, and 3D, marked by their flags")
{
  const DxfIO::Document file = Heavy();
  CHECK(Traced(file.Entities, 0x2fa).Flags == 2);
  CHECK(Traced(file.Entities, 0x303).Curve == 6);
  CHECK(Traced(file.Entities, 0x312).Curve == 5);
  CHECK(Traced(file.Entities, 0x340).Flags == 8);
  CHECK(Traced(file.Entities, 0x345).Flags == 9);
  CHECK(Traced(file.Entities, 0x34a).Flags == 12);
  CHECK(Marked(Traced(file.Entities, 0x2ce)) == 0);
  CHECK(Marked(Traced(file.Entities, 0x340)) == 1);
  CHECK(Marked(Traced(file.Entities, 0x361)) == 2);
  CHECK(Marked(Traced(file.Entities, 0x374)) == 3);
}

TEST_CASE("polyline: its thickness, flags and widths in their order")
{
  const DxfIO::Document file = Heavy();
  const Polyline &taken = Traced(file.Entities, 0x4b2);
  CHECK(taken.Thickness == 2);
  CHECK(taken.Flags == 1);
  CHECK(taken.Start == 1);
  const std::string text = Written(taken);
  CHECK(text.find(" 39\r\n") < text.find(" 70\r\n"));
  CHECK(text.find(" 70\r\n") < text.find(" 40\r\n"));
  CHECK(text.find(" 40\r\n") < text.find(" 41\r\n"));
}

TEST_CASE("polyline: a mesh's counts, smoothed and closed both ways")
{
  const DxfIO::Document file = Heavy();
  CHECK(Traced(file.Entities, 0x361).Rows == 2);
  CHECK(Traced(file.Entities, 0x361).Columns == 3);
  const Polyline &smooth = Traced(file.Entities, 0x386);
  CHECK(smooth.Density == 9);
  CHECK(smooth.Depth == 11);
  CHECK(smooth.Curve == 6);
  CHECK(Traced(file.Entities, 0x3fb).Curve == 8);
  CHECK(Traced(file.Entities, 0x470).Flags == 49);
}

TEST_CASE("polyline: a child keeps what is its own, unlike its parent")
{
  const DxfIO::Document file = Heavy();
  const Polyline &taken = Traced(file.Entities, 0x37d);
  REQUIRE(taken.Vertices.Children.size() == 3);
  CHECK(taken.Record.Layer == "0");
  CHECK(taken.Vertices.Children.front().Record.Layer == "walls");
  CHECK(taken.Vertices.Children.front().Record.Color == 1);
  REQUIRE(taken.Vertices.Closing);
  CHECK((*taken.Vertices.Closing)->Record.Layer == "walls");
  CHECK((*taken.Vertices.Closing)->Record.Color == 2);
}

TEST_CASE("polyline: extended data on the parent and on a child")
{
  const DxfIO::Document file = Heavy();
  const Polyline &taken = Traced(file.Entities, 0x48a);
  CHECK(taken.Record.Base.Extended.size() == 1);
  CHECK(taken.Vertices.Children.front().Record.Base.Extended.size() == 1);
}

TEST_CASE("polyline: extended data on a SEQEND kept, though AutoCAD makes none")
{
  const std::string end = Joined({Closing, "1001\r\nKDX\r\n1000\r\nend\r\n"});
  const auto read = Load(Drawing(Joined({Head, end})));
  REQUIRE(read);
  const auto &taken = std::get<Polyline>(read->Document.Entities.front());
  REQUIRE(taken.Vertices.Closing);
  CHECK((*taken.Vertices.Closing)->Record.Base.Extended.size() == 1);
}

TEST_CASE("polyline: in paper space, and in a block")
{
  const DxfIO::Document file = Heavy();
  CHECK(Traced(file.Entities, 0x4a6).Record.Paper);
  const auto found =
      std::ranges::find(file.Blocks, "held", [](const Blocks::Block &one) -> std::string { return one.Begin.Name; });
  REQUIRE(found != file.Blocks.end());
  REQUIRE(found->Entities.size() == 1);
  CHECK(std::get<Polyline>(found->Entities.front()).Vertices.Children.size() == 3);
}

TEST_CASE("polyline: every form reads whole in every release from R2000")
{
  for (const Version release : Releases)
  {
    CAPTURE(release);
    const auto read = Load(Acad(Fixture("heavy", release)));
    REQUIRE(read);
    CHECK(read->Issues.empty());
    CHECK(std::holds_alternative<Polyline>(read->Document.Entities.front()));
  }
}

TEST_CASE("polyline: R12's the same, with no markers and nothing left over")
{
  const auto read = Load(Acad(Fixture("heavy", Version::R12)));
  REQUIRE(read);
  CHECK(read->Issues.empty());
  const List &all = read->Document.Entities;
  CHECK(Traced(all, 0x2ce).Vertices.Children.size() == 3);
  CHECK(Traced(all, 0x361).Rows == 2);
  CHECK(Traced(all, 0x386).Density == 9);
  CHECK(Traced(all, 0x34a).Curve == 5);
  std::size_t held = 0;
  for (const Entity &one : all)
  {
    if (const auto *chain = std::get_if<Polyline>(&one))
    {
      ++held;
      CHECK(chain->Record.Base.Extra.empty());
      CHECK(chain->Vertices.Closing);
      for (const Vertex &child : chain->Vertices.Children)
      {
        CHECK(child.Record.Base.Extra.empty());
      }
    }
  }
  CHECK(held > 20);
}

TEST_CASE("polyline: no SEQEND read and told, and none written")
{
  const std::string text = Drawing(Joined({Head, Corner, Stray}));
  const auto read = Load(text);
  REQUIRE(read);
  REQUIRE(read->Document.Entities.size() == 2);
  const auto &taken = std::get<Polyline>(read->Document.Entities.front());
  CHECK(taken.Vertices.Children.size() == 1);
  CHECK_FALSE(taken.Vertices.Closing);
  REQUIRE(read->Issues.size() == 1);
  CHECK(read->Issues.front().What == "POLYLINE has no SEQEND");
  const auto saved = Tests::Written(read->Document, Options{});
  REQUIRE(saved);
  CHECK_FALSE(saved->contains("SEQEND"));
}

TEST_CASE("polyline: no SEQEND at the section's end, told")
{
  const auto read = Load(Drawing(Joined({Head, Corner, Other})));
  REQUIRE(read);
  REQUIRE(read->Document.Entities.size() == 1);
  CHECK(std::get<Polyline>(read->Document.Entities.front()).Vertices.Children.size() == 2);
  CHECK(read->Issues.size() == 1);
}

TEST_CASE("polyline: records after its SEQEND are none of its own")
{
  const auto read = Load(Drawing(Joined({Head, Corner, Closing, Other, Closing})));
  REQUIRE(read);
  const List &all = read->Document.Entities;
  REQUIRE(all.size() == 3);
  CHECK(std::get<Polyline>(all.at(0)).Vertices.Children.size() == 1);
  CHECK(std::get<Unknown>(all.at(1)).Type == "VERTEX");
  CHECK(std::get<Unknown>(all.at(2)).Type == "SEQEND");
}

TEST_CASE("polyline: a marker unlike its flags gives way to them")
{
  std::string text{Head};
  text.replace(text.find(" 30\r\n0.0\r\n"), 10, " 30\r\n0.0\r\n 70\r\n     8\r\n");
  const auto read = Load(Drawing(Joined({text, Closing})));
  REQUIRE(read);
  const auto saved = Tests::Written(read->Document, Options{});
  REQUIRE(saved);
  CHECK(saved->contains("AcDb3dPolyline"));
  CHECK_FALSE(saved->contains("AcDb2dPolyline"));
}

TEST_CASE("polyline: none of its vertices, a SEQEND alone")
{
  const auto read = Load(Drawing(Joined({Head, Closing})));
  REQUIRE(read);
  REQUIRE(read->Document.Entities.size() == 1);
  const auto &taken = std::get<Polyline>(read->Document.Entities.front());
  CHECK(taken.Vertices.Children.empty());
  CHECK(taken.Vertices.Closing);
  CHECK(read->Issues.empty());
}

TEST_CASE("polyline: its vertices held in no more room than they take, the drawing's too")
{
  const auto read = Load(Drawing(Joined({Head, Corner, Other, Corner, Other, Corner, Closing})));
  REQUIRE(read);
  const List &all = read->Document.Entities;
  REQUIRE(all.size() == 1);
  CHECK(all.capacity() == 1);
  const auto &taken = std::get<Polyline>(all.front());
  REQUIRE(taken.Vertices.Children.size() == 5);
  CHECK(taken.Vertices.Children.capacity() == 5);
}

TEST_CASE("polyline: a record between its vertices ends them, the rest stay")
{
  const std::string text = Drawing(Joined({Head, Corner, Stray, Other, Closing}));
  const auto read = Load(text);
  REQUIRE(read);
  const List &all = read->Document.Entities;
  REQUIRE(all.size() == 4);
  CHECK(std::get<Polyline>(all.at(0)).Vertices.Children.size() == 1);
  CHECK(std::holds_alternative<Line>(all.at(1)));
  CHECK(std::get<Unknown>(all.at(2)).Type == "VERTEX");
  CHECK(std::get<Unknown>(all.at(3)).Type == "SEQEND");
  const auto saved = Tests::Written(read->Document, Options{});
  REQUIRE(saved);
  CHECK(saved->contains(Joined({Stray, Other, Closing})));
}

TEST_CASE("polyline: a vertex or a SEQEND with no parent stays as it came")
{
  const auto read = Load(Drawing(Joined({Corner, Closing})));
  REQUIRE(read);
  const List &all = read->Document.Entities;
  REQUIRE(all.size() == 2);
  CHECK(std::holds_alternative<Unknown>(all.at(0)));
  CHECK(std::holds_alternative<Unknown>(all.at(1)));
}

TEST_CASE("polyline: its 66 decides nothing, the records that follow do")
{
  std::string text{Head};
  text.replace(text.find(" 66\r\n     1\r\n"), 13, " 66\r\n     0\r\n");
  const auto read = Load(Drawing(Joined({text, Corner, Closing})));
  REQUIRE(read);
  const auto &taken = std::get<Polyline>(read->Document.Entities.front());
  CHECK_FALSE(taken.Follows);
  CHECK(taken.Vertices.Children.size() == 1);
}

TEST_CASE("polyline: a new one has its 66 and point, and no other group")
{
  Polyline fresh{};
  fresh.Record.Base.Self = Handle{0x2a};
  fresh.Record.Base.Owner = Handle{0x1f};
  CHECK(Written(fresh).ends_with("100\r\nAcDb2dPolyline\r\n 66\r\n     1\r\n 10\r\n0.0\r\n 20\r\n0.0\r\n"
                                 " 30\r\n0.0\r\n"));
}
} // namespace DxfIO::Entities
