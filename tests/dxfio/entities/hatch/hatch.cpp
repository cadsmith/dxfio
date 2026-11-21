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
      REQUIRE(!one.Data->Paths.empty());
      REQUIRE(at < one.Data->Paths.front().Edges.size());
      const auto *held = std::get_if<KindT>(&one.Data->Paths.front().Edges.at(at));
      REQUIRE(held != nullptr);
      return *held;
    }
  } // namespace

  TEST_CASE("hatch: a pattern over a polyline's edges, each level its codes")
  {
    const Hatch one = Alikened(Marked("2D0"));
    CHECK(one.Data->Elevation      == Vector{});
    CHECK(one.Data->Normal         == Up);
    CHECK(one.Data->Name           == "ANSI31");
    CHECK(one.Data->Solid          == 0);
    CHECK(one.Data->Associative    == 0);
    REQUIRE(one.Data->Paths.size() == 1);
    CHECK(one.Data->Paths.front().Flags        == 1);
    CHECK(one.Data->Paths.front().Edges.size() == 4);
    CHECK(one.Data->Style == 1);
    CHECK(one.Data->Kind  == 1);
    CHECK(one.Data->Angle == 45);
    CHECK(one.Data->Scale == 2);
    REQUIRE(one.Data->PatternLines.size()      == 1);
    CHECK(one.Data->PatternLines.front().Angle == 90);
    CHECK(one.Data->Seeds.size() == 1);
    CHECK_FALSE(one.Data->Gradient);
  }

  TEST_CASE("hatch: every edge a kind of its own")
  {
    CHECK(Edge<Edges::Line>(Alikened(Marked("2D0"))).End.X != 0);
    const Edges::Arc round = Edge<Edges::Arc>(Alikened(Marked("2D3")));
    CHECK(round.Radius           == 10);
    CHECK(round.CounterClockwise == 1);
    CHECK(Edge<Edges::Ellipse>(Alikened(Marked("2D5"))).Ratio == 0.5);
    const Hatch mixed = Alikened(Marked("2D9"));
    CHECK(std::holds_alternative<Edges::Line>(mixed.Data->Paths.front().Edges.at(0)));
    CHECK(std::holds_alternative<Edges::Arc>(mixed.Data->Paths.front().Edges.at(1)));
  }

  TEST_CASE("hatch: a spline edge, its fit points and tangents from R2010")
  {
    const Hatch          one   = Alikened(Marked("2D7"));
    const Edges::Spline &curve = *Edge<Boxed<Edges::Spline>>(one);
    CHECK(curve.Degree       == 3);
    CHECK(curve.Knots.size() == 11);
    CHECK(curve.ControlPoints.size() == 7);
    CHECK(curve.FitPoints.size()     == 5);
    CHECK(curve.StartTangent.Y != 0);
    const Hatch old = Alikened(Marked("2D7", Version::R2007), Version::R2007);
    CHECK(Edge<Boxed<Edges::Spline>>(old)->FitPoints.empty());
    CHECK(Edge<Boxed<Edges::Spline>>(old)->ControlPoints.size() == 7);
  }

  TEST_CASE("hatch: a polyline boundary, its bulges given")
  {
    const Hatch one  = Alikened(Marked("2E2"));
    const Path &loop = one.Data->Paths.front();
    CHECK((loop.Flags & 2) != 0);
    CHECK(loop.Bulged            == 1);
    CHECK(loop.Closed            == 1);
    REQUIRE(loop.Vertices.size() == 4);
    CHECK(loop.Vertices.at(1).Weight == doctest::Approx(0.5));
    CHECK(loop.Edges.empty());
    CHECK(one.Data->Pixel);
  }

  TEST_CASE("hatch: a path's polyline flag written from what the path holds")
  {
    Hatch one{};
    Path  straight{};
    straight.Flags    = 1;
    straight.Closed   = 1;
    straight.Vertices = {
        Edges::Node{.Position = Vector{}},
        Edges::Node{.Position = Vector{.X = 1}},
        Edges::Node{.Position = Vector{.Y = 1}},
    };
    Path edged{};
    edged.Flags     = 18;
    edged.Edges     = {Edges::Line{.Start = Vector{}, .End = Vector{.X = 1}}};
    one.Data->Paths = {straight, edged, Path{.Flags = 2}};
    const auto back = DxfIO::Read<Hatch>(Written(one));
    REQUIRE(back.Data->Paths.size()       == 3);
    CHECK(back.Data->Paths.at(0).Flags    == 3);
    CHECK(back.Data->Paths.at(0).Vertices == straight.Vertices);
    CHECK(back.Data->Paths.at(1).Flags    == 16);
    CHECK(back.Data->Paths.at(1).Edges    == edged.Edges);
    CHECK(back.Data->Paths.at(2).Flags    == 2);
  }

  TEST_CASE("hatch: islands, a loop each and a seed each")
  {
    const Hatch one = Alikened(Marked("2F4"));
    CHECK(one.Data->Paths.size() == 3);
    CHECK(one.Data->Seeds.size() == 3);
    CHECK(one.Data->Style        == 1);
    CHECK(Alikened(Marked("30F")).Data->Style == 2);
  }

  TEST_CASE("hatch: associative, its sources by handle")
  {
    const Hatch one = Alikened(Marked("311"));
    CHECK(one.Data->Associative == 1);
    REQUIRE(one.Data->Paths.front().Sources.size() == 1);
    CHECK(one.Data->Paths.front().Sources.front() != Handle{});
  }

  TEST_CASE("hatch: patterns of several lines, doubled, dashed")
  {
    const Hatch user = Alikened(Marked("318"));
    CHECK(user.Data->Name    == "_U");
    CHECK(user.Data->Kind    == 0);
    CHECK(user.Data->Doubled == 1);
    CHECK(user.Data->PatternLines.size() == 2);
    const Hatch brick = Alikened(Marked("31C"));
    CHECK(brick.Data->PatternLines.at(1).Dashes.size() == 2);
  }

  TEST_CASE("hatch: a gradient from R2004, none on a pattern, none before")
  {
    const Hatch one = Alikened(Marked("321"));
    REQUIRE(one.Data->Gradient);
    CHECK(one.Data->Gradient->Shown           == 1);
    CHECK(one.Data->Gradient->Name            == "LINEAR");
    REQUIRE(one.Data->Gradient->Colors.size() == 2);
    CHECK(one.Data->Gradient->Colors.at(1).Place         == 1);
    CHECK(Alikened(Marked("325")).Data->Gradient->Single == 1);
    CHECK(Alikened(Marked("2D3")).Data->Gradient->Shown  == 0);
    CHECK_FALSE(Alikened(Marked("2D3", Version::R2000), Version::R2000).Data->Gradient);
  }

  TEST_CASE("hatch: every release from R2000 alike")
  {
    for (const Version release : Releases)
    {
      CAPTURE(release);
      CHECK(Alikened(Marked("2F4", release), release).Data->Paths.size() == 3);
      CHECK(Alikened(Marked("2D9", release), release).Data->Solid        == 1);
    }
  }

  TEST_CASE("hatch: extended data kept, a stray group after the fields")
  {
    const Hatch one = Alikened(Marked("32B"));
    CHECK_FALSE(one.Record.Base.Extended.empty());
    const std::string text  = Marked("2D0");
    const std::string with  = text.substr(0, text.find("1001\r\n"))
                            + " 39\r\n1.0\r\n"
                            + text.substr(text.find("1001\r\n"));
    const Hatch       stray = DxfIO::Read<Hatch>(with);
    REQUIRE(stray.Record.Base.Extra.size()     == 1);
    CHECK(stray.Record.Base.Extra.front().Code == 39);
    CHECK(stray.Data->Seeds.size()             == 1);
  }

  TEST_CASE("hatch: a record out of its order stays as it came, and is told")
  {
    const std::string text = Marked("2D0");
    const std::string from = " 75\r\n";
    const std::string late =
        text.substr(0, text.find(" 70\r\n"))
        + text.substr(text.find(" 71\r\n"), text.find(from) - text.find(" 71\r\n"))
        + " 70\r\n     0\r\n"
        + text.substr(text.find(from));
    const auto all = Tags::All(late);
    REQUIRE(all);
    File::Cursor in{*all};
    const auto   one = in.Take();
    REQUIRE(one);
    const std::vector<File::Record> records{*one};
    std::vector<Issue>              issues;
    const List                      kept =
        Entities::Read(records, Fields::Reading{.Text = Encoding::Utf8(), .Issues = &issues});
    REQUIRE(kept.size() == 1);
    CHECK(std::holds_alternative<Unknown>(kept.front()));
    CHECK(issues.size() == 1);
  }

  TEST_CASE("hatch: a new one a solid fill, as AutoCAD writes one, its spline edges cubic")
  {
    const Hatch one{};
    CHECK(one.Data->Name  == "SOLID");
    CHECK(one.Data->Solid == 1);
    CHECK(one.Data->Kind  == 1);
    CHECK(one.Data->Scale == 1);
    CHECK(Written(one).contains("  2\r\nSOLID\r\n 70\r\n     1\r\n"));
    CHECK(Edges::Spline{}.Degree == 3);
  }

  TEST_CASE("hatch: a copy is its own, its box deep")
  {
    Hatch one{};
    one.Data->Style = 1;
    Hatch two       = one;
    two.Data->Style = 2;
    CHECK(one.Data->Style == 1);
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
    const auto   one = in.Take();
    REQUIRE(one);
    const std::vector<File::Record> records{*one};
    std::vector<Issue>              issues;
    const List                      kept =
        Entities::Read(records, Fields::Reading{.Text = Encoding::Utf8(), .Issues = &issues});
    REQUIRE(kept.size() == 1);
    CHECK(std::holds_alternative<Unknown>(kept.front()));
    CHECK(issues.size() == 1);
  }

  TEST_CASE("hatch: read whole, what it told is kept")
  {
    const std::string text = Marked("2D0");
    const std::string head{"HATCH\r\n"};
    const std::size_t at      = text.find(head) + head.size();
    const std::string doubled = text.substr(0, at) + "  5\r\n2D1\r\n" + text.substr(at);
    const auto        all     = Tags::All(doubled);
    REQUIRE(all);
    File::Cursor in{*all};
    const auto   one = in.Take();
    REQUIRE(one);
    const std::vector<File::Record> records{*one};
    std::vector<Issue>              issues;
    const List                      kept =
        Entities::Read(records, Fields::Reading{.Text = Encoding::Utf8(), .Issues = &issues});
    REQUIRE(kept.size() == 1);
    CHECK(std::holds_alternative<Hatch>(kept.front()));
    REQUIRE(issues.size()     == 1);
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
    const auto   one = in.Take();
    REQUIRE(one);
    const std::vector<File::Record> records{*one};
    const List kept = Entities::Read(records, Fields::Reading{.Text = Encoding::Utf8()});
    REQUIRE(kept.size() == 1);
    CHECK(std::holds_alternative<Unknown>(kept.front()));
  }
} // namespace DxfIO::Entities
