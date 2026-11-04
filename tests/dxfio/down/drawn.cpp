#include "down/drawn.hpp"

#include "../written.hpp"
#include "down/down.hpp"

#include "dxfio/create.hpp"
#include "dxfio/load.hpp"
#include "dxfio/save.hpp"

#include <doctest/doctest.h>

#include <algorithm>
#include <cmath>
#include <limits>
#include <numbers>
#include <string>
#include <variant>
#include <vector>

namespace DxfIO::Down
{
namespace
{
constexpr double HalfTurn = std::numbers::pi;

auto Near(const Vector &one, const Vector &other) -> bool
{
  return std::abs(one.X - other.X) < 1e-9 && std::abs(one.Y - other.Y) < 1e-9 && std::abs(one.Z - other.Z) < 1e-9;
}

auto At(const Entities::Polyline &one, std::size_t index) -> Vector
{
  return one.Vertices.Children.at(index).Position;
}

auto Bezier(std::vector<double> weights = {}) -> Entities::Spline
{
  Entities::Spline out{};
  out.Curve->Degree = 2;
  out.Curve->Knots = {0, 0, 0, 1, 1, 1};
  out.Curve->Poles = {Vector{}, Vector{.X = 1, .Y = 2}, Vector{.X = 2}};
  out.Curve->Weights = std::move(weights);
  return out;
}

auto Box() -> Extents
{
  return Extents{.Low = Vector{.X = -10, .Y = -10}, .High = Vector{.X = 10, .Y = 10}};
}

auto Arced(double start, double end) -> Entities::Polyline
{
  Entities::Ellipse one{};
  one.Axis = Vector{.X = 1};
  one.Start = start;
  one.End = end;
  Seed handles;
  return Traced(one, handles);
}

auto Finite(const Entities::Vertex &one) -> bool
{
  return std::isfinite(one.Position.X) && std::isfinite(one.Position.Y) && std::isfinite(one.Position.Z);
}

auto Textual(const Entity &one) -> bool
{
  return std::holds_alternative<Entities::Text>(one);
}

// A drawing of each kind R12 has not but draws: an ellipse, a spline, a ray, a construction line, and in a block an
// MTEXT of two lines.
auto Unkinded() -> Document
{
  Document out = Create(Version::R2018);
  out.Entities.emplace_back(Entities::Ellipse{.Record = {.Base = {.Self = Handle{.Value = 0x900}}}});
  out.Entities.emplace_back(Bezier());
  out.Entities.emplace_back(Entities::Ray{});
  out.Entities.emplace_back(Entities::XLine{});
  Entities::MText note{};
  note.Paragraph->Value = "x\\Py";
  out.Blocks.front().Entities.emplace_back(note);
  return out;
}
} // namespace

TEST_CASE("drawn: an ellipse as a 3d polyline, 128 steps round")
{
  Entities::Ellipse one{};
  one.Record.Base.Self = Handle{.Value = 0x40};
  one.Center = Vector{.X = 5};
  one.Axis = Vector{.X = 2};
  one.Ratio = 0.5;
  Seed handles{Handle{.Value = 0x100}};
  const Entities::Polyline made = Traced(one, handles);
  CHECK(made.Flags == 8);
  REQUIRE(made.Vertices.Children.size() == 129);
  CHECK(made.Vertices.Children.front().Flags == 32);
  CHECK(Near(At(made, 0), Vector{.X = 7}));
  CHECK(Near(At(made, 32), Vector{.X = 5, .Y = 1}));
  CHECK(Near(At(made, 128), Vector{.X = 7}));
  CHECK(made.Vertices.Children.front().Record.Base.Owner == Handle{.Value = 0x40});
  REQUIRE(made.Vertices.Closing);
}

TEST_CASE("drawn: an arc of an ellipse, its end past a turn, its plane tilted")
{
  Entities::Ellipse half{};
  half.Axis = Vector{.X = 1};
  half.Start = HalfTurn;
  half.End = 0;
  Seed handles;
  const Entities::Polyline made = Traced(half, handles);
  REQUIRE(made.Vertices.Children.size() == 65);
  CHECK(Near(At(made, 64), Vector{.X = 1}));
  Entities::Ellipse flat{};
  flat.Axis = Vector{.X = 1};
  flat.Extrusion = Vector{};
  CHECK(Near(At(Traced(flat, handles), 32), Vector{.Y = 1}));
  Entities::Ellipse tilted{};
  tilted.Axis = Vector{.X = 1};
  tilted.Extrusion = Vector{.Y = -2};
  CHECK(Near(At(Traced(tilted, handles), 32), Vector{.Z = 1}));
  Entities::Ellipse sliver{};
  sliver.Start = 0;
  sliver.End = 1e-9;
  CHECK(Traced(sliver, handles).Vertices.Children.size() == 3);
}

TEST_CASE("drawn: an ellipse of angles far off or not numbers still ends")
{
  CHECK(Arced(1e17, 1e17).Vertices.Children.size() == 129);
  CHECK(Arced(0, -1e15).Vertices.Children.size() <= 129);
  CHECK(Arced(1e17, 0).Vertices.Children.size() <= 129);
  CHECK(Arced(-HalfTurn, -HalfTurn - (HalfTurn / 2)).Vertices.Children.size() == 97);
  CHECK(Arced(0, std::numeric_limits<double>::infinity()).Vertices.Children.size() == 129);
  const Entities::Polyline lost = Arced(std::numeric_limits<double>::quiet_NaN(), HalfTurn);
  REQUIRE(lost.Vertices.Children.size() == 129);
  CHECK(std::ranges::all_of(lost.Vertices.Children, Finite));
  CHECK(Near(At(lost, 0), Vector{.X = 1}));
}

TEST_CASE("drawn: a spline along its curve, weighted or not")
{
  Seed handles;
  const Entities::Polyline plain = Traced(Bezier(), handles);
  REQUIRE(plain.Vertices.Children.size() == 129);
  CHECK(Near(At(plain, 0), Vector{}));
  CHECK(Near(At(plain, 64), Vector{.X = 1, .Y = 1}));
  CHECK(Near(At(plain, 128), Vector{.X = 2}));
  const Entities::Polyline heavy = Traced(Bezier({1, 2, 1}), handles);
  CHECK(Near(At(heavy, 64), Vector{.X = 1, .Y = 4.0 / 3}));
  const Entities::Polyline zero = Traced(Bezier({1, 0, 1}), handles);
  CHECK(Near(At(zero, 64), Vector{.X = 1}));
  const Entities::Polyline none = Traced(Bezier({0, 0, 0}), handles);
  CHECK(Near(At(none, 64), Vector{}));
}

TEST_CASE("drawn: a spline of fit points only, or of knots that do not fit")
{
  Seed handles;
  Entities::Spline fitted{};
  fitted.Curve->Fits = {Vector{.X = 1}, Vector{.X = 2}, Vector{.X = 3}};
  CHECK(Traced(fitted, handles).Vertices.Children.size() == 3);
  Entities::Spline broken = Bezier();
  broken.Curve->Knots = {0, 1};
  const Entities::Polyline poles = Traced(broken, handles);
  REQUIRE(poles.Vertices.Children.size() == 3);
  CHECK(Near(At(poles, 1), Vector{.X = 1, .Y = 2}));
  Entities::Spline unsorted = Bezier();
  unsorted.Curve->Knots = {0, 0, 1, 0, 1, 1};
  CHECK(Traced(unsorted, handles).Vertices.Children.size() == 3);
  Entities::Spline point = Bezier();
  point.Curve->Knots = {0, 0, 0, 0, 0, 0};
  CHECK(Traced(point, handles).Vertices.Children.size() == 3);
  Entities::Spline flat = Bezier();
  flat.Curve->Degree = 0;
  CHECK(Traced(flat, handles).Vertices.Children.size() == 3);
  Entities::Spline few = Bezier();
  few.Curve->Degree = 3;
  CHECK(Traced(few, handles).Vertices.Children.size() == 3);
}

TEST_CASE("drawn: a spline of several spans, 128 steps at least, 16 a span")
{
  Entities::Spline out{};
  out.Curve->Degree = 1;
  out.Curve->Knots = {0, 0, 0.5, 0.5, 1, 1};
  out.Curve->Poles = {Vector{}, Vector{.X = 1}, Vector{.X = 1, .Y = 1}, Vector{.Y = 1}};
  Seed handles;
  const Entities::Polyline made = Traced(out, handles);
  REQUIRE(made.Vertices.Children.size() == 129);
  CHECK(Near(At(made, 128), Vector{.Y = 1}));
  Entities::Spline many{};
  many.Curve->Degree = 1;
  many.Curve->Knots = {0};
  for (int at = 0; at <= 10; ++at)
  {
    many.Curve->Poles.push_back(Vector{.X = static_cast<double>(at)});
    many.Curve->Knots.push_back(static_cast<double>(at));
  }
  many.Curve->Knots.push_back(10);
  CHECK(Traced(many, handles).Vertices.Children.size() == 161);
}

TEST_CASE("drawn: a spline whose end knot repeats past its degree ends on the last span")
{
  Entities::Spline out{};
  out.Curve->Degree = 1;
  out.Curve->Knots = {0, 0, 1, 1, 1};
  out.Curve->Poles = {Vector{}, Vector{.X = 1, .Y = 2}, Vector{.X = 2}};
  Seed handles;
  const Entities::Polyline made = Traced(out, handles);
  REQUIRE(made.Vertices.Children.size() == 129);
  CHECK(Near(At(made, 64), Vector{.X = 0.5, .Y = 1}));
  CHECK(Near(At(made, 128), Vector{.X = 1, .Y = 2}));
}

TEST_CASE("drawn: a ray cut where it leaves the drawing")
{
  Entities::Ray ray{};
  ray.Record.Base.Self = Handle{.Value = 0x50};
  ray.Direction = Vector{.X = 1, .Y = 1};
  const Entities::Line one = Clipped(ray, Box());
  CHECK(Near(one.Start, Vector{}));
  CHECK(Near(one.End, Vector{.X = 10, .Y = 10}));
  CHECK(one.Record.Base.Self == Handle{.Value = 0x50});
  ray.Start = Vector{.X = -20};
  ray.Direction = Vector{.X = 1};
  const Entities::Line enter = Clipped(ray, Box());
  CHECK(Near(enter.Start, Vector{.X = -20}));
  CHECK(Near(enter.End, Vector{.X = 10}));
  ray.Direction = Vector{.X = -1};
  CHECK(Near(Clipped(ray, Box()).End, Vector{.X = -1020}));
}

TEST_CASE("drawn: a construction line cut both ways, or a reach around it")
{
  Entities::XLine line{};
  line.Start = Vector{.X = 1};
  line.Direction = Vector{.Y = 1};
  const Entities::Line one = Clipped(line, Box());
  CHECK(Near(one.Start, Vector{.X = 1, .Y = -10}));
  CHECK(Near(one.End, Vector{.X = 1, .Y = 10}));
  const Entities::Line far = Clipped(line, Extents{});
  CHECK(Near(far.Start, Vector{.X = 1, .Y = -1000}));
  CHECK(Near(far.End, Vector{.X = 1, .Y = 1000}));
  line.Direction = Vector{};
  CHECK(Near(Clipped(line, Box()).Start, Vector{.X = 1}));
}

TEST_CASE("drawn: the extents a drawing declares, when it declares them")
{
  Document drawing = Create(Version::R2018);
  for (Header::Variable &one : drawing.Header)
  {
    if (one.Name == "$EXTMIN")
    {
      one.Values = {Group{.Code = 10, .Value = 1.0}, Group{.Code = 20, .Value = 2.0}, Group{.Code = 30, .Value = 3.0}};
    }
  }
  const Extents box = Bounded(drawing);
  CHECK(box.Low == Vector{.X = 1, .Y = 2, .Z = 3});
  std::erase_if(drawing.Header, [](const Header::Variable &one) -> bool { return one.Name == "$EXTMAX"; });
  CHECK(Bounded(drawing).Low == Vector{});
  for (Header::Variable &one : drawing.Header)
  {
    if (one.Name == "$EXTMIN")
    {
      one.Values = {Group{.Code = 10, .Value = 1.0}};
    }
  }
  CHECK(Bounded(drawing).High == Vector{});
}

TEST_CASE("drawn: MTEXT codes stripped to the words they show")
{
  CHECK(Plain("a\\Pb") == std::vector<std::string>{"a", "b"});
  CHECK(Plain("{\\fArial|b1;Bold} x") == std::vector<std::string>{"Bold x"});
  CHECK(Plain("\\S1^2; and \\S3#4") == std::vector<std::string>{"1/2 and 3/4"});
  CHECK(Plain("a\\~b") == std::vector<std::string>{"a b"});
  CHECK(Plain("\\\\ \\{x\\}") == std::vector<std::string>{"\\ {x}"});
  CHECK(Plain("\\Lunder\\l") == std::vector<std::string>{"under"});
  CHECK(Plain("\\H2.5x;big") == std::vector<std::string>{"big"});
  CHECK(Plain("\\Hnever") == std::vector<std::string>{""});
  CHECK(Plain("one\ntwo") == std::vector<std::string>{"one", "two"});
  CHECK(Plain("end\\") == std::vector<std::string>{"end\\"});
}

TEST_CASE("drawn: MTEXT as a line of TEXT each, placed by its attachment")
{
  Entities::MText one{};
  one.Record.Base.Self = Handle{.Value = 0x60};
  one.Record.Base.Owner = Handle{.Value = 0x1f};
  one.Record.Layer = Symbol{"Notes"};
  Entities::Paragraph &body = *one.Paragraph;
  body.Position = Vector{.X = 10, .Y = 10};
  body.Height = 3;
  body.Value = "a\\Pb\\Pc";
  body.Style = Symbol{"Romans"};
  Seed handles{Handle{.Value = 0x200}};
  const std::vector<Entities::Text> top = Lettered(one, handles);
  REQUIRE(top.size() == 3);
  CHECK(top.front().Record.Base.Self == Handle{.Value = 0x60});
  CHECK(top.at(1).Record.Base.Self == Handle{.Value = 0x200});
  CHECK(top.at(1).Record.Base.Owner == Handle{.Value = 0x1f});
  CHECK(top.at(1).Record.Layer == "Notes");
  CHECK(top.front().Vertical == 3);
  CHECK(top.front().Lettering.Horizontal == 0);
  CHECK(top.front().Lettering.Style == "Romans");
  CHECK(Near(top.front().Lettering.Position, Vector{.X = 10, .Y = 10}));
  CHECK(Near(top.at(2).Lettering.Alignment.value_or(Vector{}), Vector{.X = 10, .Y = 0}));
  body.Attachment = 5;
  const std::vector<Entities::Text> middle = Lettered(one, handles);
  CHECK(middle.front().Vertical == 2);
  CHECK(middle.front().Lettering.Horizontal == 1);
  CHECK(Near(middle.at(1).Lettering.Position, Vector{.X = 10, .Y = 10}));
  body.Attachment = 9;
  const std::vector<Entities::Text> bottom = Lettered(one, handles);
  CHECK(bottom.front().Vertical == 1);
  CHECK(Near(bottom.at(2).Lettering.Position, Vector{.X = 10, .Y = 10}));
  body.Attachment = 0;
  CHECK(Lettered(one, handles).front().Vertical == 3);
  body.Attachment = 10;
  CHECK(Lettered(one, handles).front().Lettering.Horizontal == 0);
}

TEST_CASE("drawn: MTEXT turned by its direction, else by its rotation")
{
  Entities::MText one{};
  one.Paragraph->Height = 3;
  one.Paragraph->Value = "a\\Pb";
  one.Paragraph->Direction = Vector{.Y = 1};
  Seed handles;
  const std::vector<Entities::Text> turned = Lettered(one, handles);
  CHECK(std::abs(turned.front().Lettering.Rotation - 90) < 1e-9);
  CHECK(Near(turned.at(1).Lettering.Position, Vector{.X = 5}));
  one.Paragraph->Direction.reset();
  one.Paragraph->Rotation = HalfTurn;
  CHECK(std::abs(Lettered(one, handles).front().Lettering.Rotation - 180) < 1e-9);
}

TEST_CASE("drawn: MTEXT placed in the TEXT's own coordinate system")
{
  Entities::MText one{};
  one.Paragraph->Height = 3;
  one.Paragraph->Value = "a\\Pb";
  one.Paragraph->Position = Vector{.X = 10};
  one.Paragraph->Direction = Vector{.X = 1};
  one.Paragraph->Extrusion = Vector{.Z = -1};
  Seed handles;
  const std::vector<Entities::Text> under = Lettered(one, handles);
  REQUIRE(under.size() == 2);
  CHECK(Near(under.front().Lettering.Position, Vector{.X = -10}));
  CHECK(std::abs(under.front().Lettering.Rotation - 180) < 1e-9);
  CHECK(Near(under.at(1).Lettering.Position, Vector{.X = -10, .Y = 5}));
  CHECK(under.front().Lettering.Extrusion == Vector{.Z = -1});
  one.Paragraph->Direction.reset();
  CHECK(std::abs(Lettered(one, handles).front().Lettering.Rotation) < 1e-9);
}

TEST_CASE("drawn: the arbitrary axes of a normal, near the z axis or not")
{
  const Axes upward = Arbitrary(Up);
  CHECK(upward.X == Vector{.X = 1});
  CHECK(upward.Y == Vector{.Y = 1});
  CHECK(upward.Z == Up);
  CHECK(Arbitrary(Vector{}).X == Vector{.X = 1});
  const Axes down = Arbitrary(Vector{.Z = -2});
  CHECK(Near(down.X, Vector{.X = -1}));
  CHECK(Near(down.Y, Vector{.Y = 1}));
  CHECK(Near(down.Z, Vector{.Z = -1}));
  const Axes east = Arbitrary(Vector{.X = 1});
  CHECK(Near(east.X, Vector{.Y = 1}));
  CHECK(Near(east.Y, Vector{.Z = 1}));
  const Axes north = Arbitrary(Vector{.Y = 1});
  CHECK(Near(north.X, Vector{.X = -1}));
  CHECK(Near(north.Y, Vector{.Z = 1}));
}

TEST_CASE("drawn: R12 draws what it has no kind for, each told")
{
  const Document from = Unkinded();
  std::vector<Issue> told;
  const Document out = Narrowed(from, Version::R12, &told);
  REQUIRE(told.size() == 5);
  CHECK(told.front().What == "MTEXT drawn as TEXT for R12");
  CHECK(told.at(1).What == "ELLIPSE drawn as POLYLINE for R12");
  CHECK(told.at(1).Record == Handle{.Value = 0x900});
  CHECK(told.back().What == "XLINE drawn as LINE for R12");
  CHECK(std::holds_alternative<Entities::Polyline>(out.Entities.at(out.Entities.size() - 4)));
  CHECK(std::holds_alternative<Entities::Line>(out.Entities.back()));
  CHECK(std::ranges::count_if(out.Blocks.front().Entities, Textual) == 2);
}

TEST_CASE("drawn: what R12 draws reads back, and R13 draws nothing")
{
  const Document from = Unkinded();
  const auto saved = Tests::Written(from, Options{.Release = Version::R12});
  REQUIRE(saved);
  const auto back = Load(*saved);
  REQUIRE(back);
  CHECK(back->Issues.empty());
  CHECK(Narrowed(from, Version::R13, nullptr).Entities.size() == from.Entities.size());
  CHECK(Narrowed(from, Version::R12, nullptr).Entities.size() == back->Document.Entities.size());
}
} // namespace DxfIO::Down

namespace DxfIO::Down
{
TEST_CASE("drawn: extents of other groups or of four reals take the first three")
{
  Document drawing = Create(Version::R2018);
  for (Header::Variable &one : drawing.Header)
  {
    if (one.Name == "$EXTMIN")
    {
      one.Values = {Group{.Code = 1, .Value = std::string{"x"}}, Group{.Code = 10, .Value = 1.0},
                    Group{.Code = 20, .Value = 2.0}, Group{.Code = 30, .Value = 3.0}, Group{.Code = 40, .Value = 4.0}};
    }
    if (one.Name == "$EXTMAX")
    {
      one.Values = {Group{.Code = 10, .Value = 5.0}, Group{.Code = 20, .Value = 6.0}};
    }
  }
  const Extents box = Bounded(drawing);
  CHECK(box.Low == Vector{.X = 1, .Y = 2, .Z = 3});
  CHECK(box.High == Vector{.X = 5, .Y = 6});
}

TEST_CASE("drawn: extents flat or upside down reach around the line instead")
{
  Entities::XLine line{};
  line.Direction = Vector{.X = 1};
  const Extents flat{.Low = Vector{.X = -10, .Y = 5}, .High = Vector{.X = 10, .Y = 5}};
  CHECK(Near(Clipped(line, flat).End, Vector{.X = 1000}));
  const Extents sunk{.Low = Vector{.X = -10, .Y = -10, .Z = 5}, .High = Vector{.X = 10, .Y = 10}};
  CHECK(Near(Clipped(line, sunk).End, Vector{.X = 1000}));
  Entities::Ray still{};
  still.Start = Vector{.X = 3};
  still.Direction = Vector{};
  CHECK(Near(Clipped(still, Box()).End, Vector{.X = 3}));
}
} // namespace DxfIO::Down
