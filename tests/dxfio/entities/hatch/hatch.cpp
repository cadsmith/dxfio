#include "entities/shapes/hatch/hatch.hpp"

#include "../../alike.hpp"
#include "../../kind.hpp"
#include "../../releases.hpp"
#include "../drawn.hpp"
#include "entities/io.hpp"
#include "file/cursor.hpp"

#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace DxfIO::Entities
{
namespace
{
auto Marked(std::string_view self, Version release = Version::R2018) -> std::string
{
  return Drawn(self, release, "hatches");
}

auto Alikened(const std::string &record, Version release = Version::R2018) -> Hatch
{
  const Hatch out = DxfIO::Read<Hatch>(record, release);
  CHECK(out.Record.Base.Extra.empty());
  CHECK(Alike(Written(out, release), record));
  return out;
}

template <class KindT>
auto Edge(const Hatch &one, std::size_t at = 0) -> const KindT &
{
  REQUIRE(!one.Fill->Paths.empty());
  REQUIRE(at < one.Fill->Paths.front().Edges.size());
  const auto *held = std::get_if<KindT>(&one.Fill->Paths.front().Edges.at(at));
  REQUIRE(held != nullptr);
  return *held;
}
} // namespace

TEST_CASE("hatch: a pattern over a polyline's edges, each level its codes")
{
  const Hatch one = Alikened(Marked("2D0"));
  CHECK(one.Fill->Elevation == Vector{});
  CHECK(one.Fill->Normal == Up);
  CHECK(one.Fill->Name == "ANSI31");
  CHECK(one.Fill->Solid == 0);
  CHECK(one.Fill->Associative == 0);
  REQUIRE(one.Fill->Paths.size() == 1);
  CHECK(one.Fill->Paths.front().Flags == 1);
  CHECK(one.Fill->Paths.front().Edges.size() == 4);
  CHECK(one.Fill->Style == 1);
  CHECK(one.Fill->Kind == 1);
  CHECK(one.Fill->Angle == 45);
  CHECK(one.Fill->Scale == 2);
  REQUIRE(one.Fill->Stripes.size() == 1);
  CHECK(one.Fill->Stripes.front().Angle == 90);
  CHECK(one.Fill->Seeds.size() == 1);
  CHECK_FALSE(one.Fill->Gradient);
}

TEST_CASE("hatch: every edge a kind of its own")
{
  CHECK(Edge<Edges::Line>(Alikened(Marked("2D0"))).End.X != 0);
  const Edges::Arc round = Edge<Edges::Arc>(Alikened(Marked("2D3")));
  CHECK(round.Radius == 10);
  CHECK(round.Counter == 1);
  CHECK(Edge<Edges::Ellipse>(Alikened(Marked("2D5"))).Ratio == 0.5);
  const Hatch mixed = Alikened(Marked("2D9"));
  CHECK(std::holds_alternative<Edges::Line>(mixed.Fill->Paths.front().Edges.at(0)));
  CHECK(std::holds_alternative<Edges::Arc>(mixed.Fill->Paths.front().Edges.at(1)));
}

TEST_CASE("hatch: a spline edge, its fit points and tangents from R2010")
{
  const Hatch one = Alikened(Marked("2D7"));
  const Edges::Spline &curve = *Edge<Boxed<Edges::Spline>>(one);
  CHECK(curve.Degree == 3);
  CHECK(curve.Knots.size() == 11);
  CHECK(curve.Poles.size() == 7);
  CHECK(curve.Fits.size() == 5);
  CHECK(curve.Start.Y != 0);
  const Hatch old = Alikened(Marked("2D7", Version::R2007), Version::R2007);
  CHECK(Edge<Boxed<Edges::Spline>>(old)->Fits.empty());
  CHECK(Edge<Boxed<Edges::Spline>>(old)->Poles.size() == 7);
}

TEST_CASE("hatch: a polyline boundary, its bulges given")
{
  const Hatch one = Alikened(Marked("2E2"));
  const Path &loop = one.Fill->Paths.front();
  CHECK((loop.Flags & 2) != 0);
  CHECK(loop.Bulged == 1);
  CHECK(loop.Closed == 1);
  REQUIRE(loop.Vertices.size() == 4);
  CHECK(loop.Vertices.at(1).Weight == doctest::Approx(0.5));
  CHECK(loop.Edges.empty());
  CHECK(one.Fill->Pixel);
}

TEST_CASE("hatch: a path's polyline flag written from what the path holds")
{
  Hatch one{};
  Path straight{};
  straight.Flags = 1;
  straight.Closed = 1;
  straight.Vertices = {
      Edges::Node{.Position = Vector{}},
      Edges::Node{.Position = Vector{.X = 1}},
      Edges::Node{.Position = Vector{.Y = 1}},
  };
  Path edged{};
  edged.Flags = 18;
  edged.Edges = {Edges::Line{.Start = Vector{}, .End = Vector{.X = 1}}};
  one.Fill->Paths = {straight, edged, Path{.Flags = 2}};
  const auto back = DxfIO::Read<Hatch>(Written(one));
  REQUIRE(back.Fill->Paths.size() == 3);
  CHECK(back.Fill->Paths.at(0).Flags == 3);
  CHECK(back.Fill->Paths.at(0).Vertices == straight.Vertices);
  CHECK(back.Fill->Paths.at(1).Flags == 16);
  CHECK(back.Fill->Paths.at(1).Edges == edged.Edges);
  CHECK(back.Fill->Paths.at(2).Flags == 2);
}

TEST_CASE("hatch: islands, a loop each and a seed each")
{
  const Hatch one = Alikened(Marked("2F4"));
  CHECK(one.Fill->Paths.size() == 3);
  CHECK(one.Fill->Seeds.size() == 3);
  CHECK(one.Fill->Style == 1);
  CHECK(Alikened(Marked("30F")).Fill->Style == 2);
}

TEST_CASE("hatch: associative, its sources by handle")
{
  const Hatch one = Alikened(Marked("311"));
  CHECK(one.Fill->Associative == 1);
  REQUIRE(one.Fill->Paths.front().Sources.size() == 1);
  CHECK(one.Fill->Paths.front().Sources.front() != Handle{});
}

TEST_CASE("hatch: patterns of several lines, doubled, dashed")
{
  const Hatch user = Alikened(Marked("318"));
  CHECK(user.Fill->Name == "_U");
  CHECK(user.Fill->Kind == 0);
  CHECK(user.Fill->Doubled == 1);
  CHECK(user.Fill->Stripes.size() == 2);
  const Hatch brick = Alikened(Marked("31C"));
  CHECK(brick.Fill->Stripes.at(1).Dashes.size() == 2);
}

TEST_CASE("hatch: a gradient from R2004, none on a pattern, none before")
{
  const Hatch one = Alikened(Marked("321"));
  REQUIRE(one.Fill->Gradient);
  CHECK(one.Fill->Gradient->Shown == 1);
  CHECK(one.Fill->Gradient->Name == "LINEAR");
  REQUIRE(one.Fill->Gradient->Stops.size() == 2);
  CHECK(one.Fill->Gradient->Stops.at(1).Place == 1);
  CHECK(Alikened(Marked("325")).Fill->Gradient->Single == 1);
  CHECK(Alikened(Marked("2D3")).Fill->Gradient->Shown == 0);
  CHECK_FALSE(Alikened(Marked("2D3", Version::R2000), Version::R2000).Fill->Gradient);
}

TEST_CASE("hatch: every release from R2000 alike")
{
  for (const Version release : Releases)
  {
    CAPTURE(release);
    CHECK(Alikened(Marked("2F4", release), release).Fill->Paths.size() == 3);
    CHECK(Alikened(Marked("2D9", release), release).Fill->Solid == 1);
  }
}

TEST_CASE("hatch: extended data kept, a stray group after the fields")
{
  const Hatch one = Alikened(Marked("32B"));
  CHECK_FALSE(one.Record.Base.Extended.empty());
  const std::string text = Marked("2D0");
  const std::string with =
      text.substr(0, text.find("1001\r\n")) + " 39\r\n1.0\r\n" + text.substr(text.find("1001\r\n"));
  const Hatch stray = DxfIO::Read<Hatch>(with);
  REQUIRE(stray.Record.Base.Extra.size() == 1);
  CHECK(stray.Record.Base.Extra.front().Code == 39);
  CHECK(stray.Fill->Seeds.size() == 1);
}

TEST_CASE("hatch: a record out of its order stays as it came, and is told")
{
  const std::string text = Marked("2D0");
  const std::string from = " 75\r\n";
  const std::string late = text.substr(0, text.find(" 70\r\n")) +
                           text.substr(text.find(" 71\r\n"), text.find(from) - text.find(" 71\r\n")) +
                           " 70\r\n     0\r\n" + text.substr(text.find(from));
  const auto all = Tags::All(late);
  REQUIRE(all);
  File::Cursor in{*all};
  const auto one = in.Take();
  REQUIRE(one);
  const std::vector<File::Record> records{*one};
  std::vector<Issue> issues;
  const List kept = Entities::Read(records, Fields::Reading{.Text = Encoding::Utf8(), .Issues = &issues});
  REQUIRE(kept.size() == 1);
  CHECK(std::holds_alternative<Unknown>(kept.front()));
  CHECK(issues.size() == 1);
}

TEST_CASE("hatch: a new one a solid fill, as AutoCAD writes one, its spline edges cubic")
{
  const Hatch one{};
  CHECK(one.Fill->Name == "SOLID");
  CHECK(one.Fill->Solid == 1);
  CHECK(one.Fill->Kind == 1);
  CHECK(one.Fill->Scale == 1);
  CHECK(Written(one).contains("  2\r\nSOLID\r\n 70\r\n     1\r\n"));
  CHECK(Edges::Spline{}.Degree == 3);
}

TEST_CASE("hatch: a copy is its own, its box deep")
{
  Hatch one{};
  one.Fill->Style = 1;
  Hatch two = one;
  two.Fill->Style = 2;
  CHECK(one.Fill->Style == 1);
}

TEST_CASE("hatch: an entity stays the size of the small kinds")
{
  CHECK(sizeof(Hatch) < 2 * sizeof(Record));
}

TEST_CASE("hatch: an edge the size of an elliptic arc, a spline's boxed")
{
  CHECK(sizeof(Edges::Edge) < sizeof(Edges::Spline));
  CHECK(sizeof(Edges::Edge) <= sizeof(Edges::Ellipse) + sizeof(std::size_t));
}

TEST_CASE("hatch: named as DXF names it")
{
  CHECK(Named(Entity{Hatch{}}) == "HATCH");
}

TEST_CASE("hatch: an edge of no type stays as it came")
{
  const auto all = Tags::All("  0\nHATCH\n100\nAcDbEntity\n  8\n0\n100\nAcDbHatch\n 10\n0.0\n"
                             " 20\n0.0\n 30\n0.0\n210\n0.0\n220\n0.0\n230\n1.0\n  2\nSOLID\n"
                             " 70\n1\n 71\n0\n 91\n1\n 92\n1\n 93\n1\n 72\n9\n");
  REQUIRE(all);
  File::Cursor in{*all};
  const auto one = in.Take();
  REQUIRE(one);
  const std::vector<File::Record> records{*one};
  std::vector<Issue> issues;
  const List kept = Entities::Read(records, Fields::Reading{.Text = Encoding::Utf8(), .Issues = &issues});
  REQUIRE(kept.size() == 1);
  CHECK(std::holds_alternative<Unknown>(kept.front()));
  CHECK(issues.size() == 1);
}

TEST_CASE("hatch: read whole, what it told is kept")
{
  const std::string text = Marked("2D0");
  const std::string head{"HATCH\r\n"};
  const std::size_t at = text.find(head) + head.size();
  const std::string doubled = text.substr(0, at) + "  5\r\n2D1\r\n" + text.substr(at);
  const auto all = Tags::All(doubled);
  REQUIRE(all);
  File::Cursor in{*all};
  const auto one = in.Take();
  REQUIRE(one);
  const std::vector<File::Record> records{*one};
  std::vector<Issue> issues;
  const List kept = Entities::Read(records, Fields::Reading{.Text = Encoding::Utf8(), .Issues = &issues});
  REQUIRE(kept.size() == 1);
  CHECK(std::holds_alternative<Hatch>(kept.front()));
  REQUIRE(issues.size() == 1);
  CHECK(issues.front().What == "group 5 given again, one kept");
}
} // namespace DxfIO::Entities

namespace DxfIO::Entities
{
TEST_CASE("hatch: an edge of type 0 stays as it came")
{
  const auto all = Tags::All("  0\nHATCH\n100\nAcDbEntity\n  8\n0\n100\nAcDbHatch\n 10\n0.0\n"
                             " 20\n0.0\n 30\n0.0\n210\n0.0\n220\n0.0\n230\n1.0\n  2\nSOLID\n"
                             " 70\n1\n 71\n0\n 91\n1\n 92\n1\n 93\n1\n 72\n0\n");
  REQUIRE(all);
  File::Cursor in{*all};
  const auto one = in.Take();
  REQUIRE(one);
  const std::vector<File::Record> records{*one};
  const List kept = Entities::Read(records, Fields::Reading{.Text = Encoding::Utf8()});
  REQUIRE(kept.size() == 1);
  CHECK(std::holds_alternative<Unknown>(kept.front()));
}
} // namespace DxfIO::Entities
