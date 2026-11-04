#include "entities/shapes/spline.hpp"

#include "../alike.hpp"
#include "../kind.hpp"
#include "../releases.hpp"
#include "drawn.hpp"

#include <limits>
#include <string>
#include <string_view>
#include <vector>

namespace DxfIO::Entities
{
namespace
{
auto Marked(std::string_view self, Version release = Version::R2018) -> std::string
{
  return Drawn(self, release, "splines");
}

auto Alikened(const std::string &record, Version release = Version::R2018) -> Spline
{
  const Spline out = DxfIO::Read<Spline>(record, release);
  CHECK(out.Record.Base.Extra.empty());
  CHECK(Alike(Written(out, release), record));
  return out;
}

auto Held(std::string_view body) -> std::string
{
  return "  0\r\nSPLINE\r\n  5\r\n2A\r\n330\r\n1F\r\n100\r\nAcDbEntity\r\n"
         "  8\r\n0\r\n100\r\nAcDbSpline\r\n" +
         std::string{body};
}
} // namespace

TEST_CASE("spline: by its control points, each of its lists")
{
  const Spline one = Alikened(Marked("2D5"));
  CHECK(one.Curve->Normal == Up);
  CHECK(one.Curve->Flags == 8);
  CHECK(one.Curve->Degree == 3);
  CHECK(one.Curve->Knotted == Knotting);
  CHECK(one.Curve->Framed == Framing);
  CHECK_FALSE(one.Curve->Fitted);
  CHECK_FALSE(one.Curve->Start);
  CHECK(one.Curve->Knots.size() == 8);
  REQUIRE(one.Curve->Poles.size() == 4);
  CHECK(one.Curve->Poles.at(1) == Vector{.X = 50, .Y = 10});
  CHECK(one.Curve->Weights.empty());
  CHECK(one.Curve->Fits.empty());
}

TEST_CASE("spline: fit by its points, what they were worked into beside them")
{
  const Spline one = Alikened(Marked("2CD"));
  CHECK(one.Curve->Flags == 1064);
  CHECK(one.Curve->Fitted == Framing);
  CHECK(one.Curve->Knots.size() == 10);
  CHECK(one.Curve->Poles.size() == 6);
  REQUIRE(one.Curve->Fits.size() == 4);
  CHECK(one.Curve->Fits.at(3) == Vector{.X = 30, .Y = 10});
  CHECK(Alikened(Marked("2CF")).Curve->Fitted == 0.5);
}

TEST_CASE("spline: its tangents where they were given")
{
  const Spline one = Alikened(Marked("2CE"));
  CHECK(one.Curve->Start);
  CHECK(one.Curve->End);
  CHECK_FALSE(Alikened(Marked("2CD")).Curve->Start);
}

TEST_CASE("spline: rational, each weight after its point")
{
  const Spline one = Alikened(Marked("2D7"));
  CHECK(one.Curve->Flags == 12);
  CHECK(one.Curve->Degree == 2);
  CHECK(one.Curve->Weights == std::vector{1.0, 0.5, 1.0});
}

TEST_CASE("spline: closed, periodic, linear and of any degree")
{
  CHECK(Alikened(Marked("2D0")).Curve->Flags == 1067);
  CHECK(Alikened(Marked("2D4")).Curve->Flags == 11);
  CHECK(Alikened(Marked("2D8")).Curve->Flags == 11);
  CHECK(Alikened(Marked("2D9")).Curve->Flags == 24);
  CHECK(Alikened(Marked("2DA")).Curve->Degree == 1);
  CHECK(Alikened(Marked("2D6")).Curve->Degree == 2);
}

TEST_CASE("spline: in no plane no normal, one given worked out again")
{
  CHECK_FALSE(Alikened(Marked("2D2")).Curve->Normal);
  CHECK_FALSE(Alikened(Marked("2DB")).Curve->Normal);
  CHECK(Alikened(Marked("2DC")).Curve->Normal == Up);
}

TEST_CASE("spline: its points purged, fit by nothing")
{
  const Spline one = Alikened(Marked("2D3"));
  CHECK(one.Curve->Flags == 8);
  CHECK(one.Curve->Fits.empty());
}

TEST_CASE("spline: every release from R2000 alike")
{
  for (const Version release : Releases)
  {
    CAPTURE(release);
    CHECK(Alikened(Marked("2CE", release), release).Curve->Degree == 3);
    CHECK(Alikened(Marked("2D7", release), release).Curve->Weights.size() == 3);
  }
}

TEST_CASE("spline: the counts ignored, each list its own groups")
{
  const Spline one = DxfIO::Read<Spline>(Held(" 71\r\n     1\r\n 72\r\n     1\r\n 73\r\n     9\r\n 74\r\n     7\r\n"
                                              " 40\r\n0.0\r\n 40\r\n0.0\r\n 40\r\n1.0\r\n 40\r\n1.0\r\n"
                                              " 10\r\n0.0\r\n 20\r\n0.0\r\n 10\r\n1.0\r\n 20\r\n0.0\r\n"));
  CHECK(one.Curve->Knots.size() == 4);
  CHECK(one.Curve->Poles.size() == 2);
  const std::string out = Written(one);
  CHECK(out.contains(" 72\r\n     4\r\n 73\r\n     2\r\n 74\r\n     0\r\n"));
}

TEST_CASE("spline: weights all after the points, written each after its own")
{
  const Spline one = DxfIO::Read<Spline>(Held(" 70\r\n     4\r\n 71\r\n     1\r\n 10\r\n0.0\r\n 20\r\n0.0\r\n"
                                              " 10\r\n1.0\r\n 20\r\n0.0\r\n 41\r\n2.0\r\n 41\r\n3.0\r\n"));
  CHECK(one.Curve->Weights == std::vector{2.0, 3.0});
  CHECK(Written(one).contains(" 41\r\n2.0\r\n 10\r\n1.0\r\n"));
}

TEST_CASE("spline: a weight past the last point stays after it")
{
  const Spline one = DxfIO::Read<Spline>(Held(" 71\r\n     1\r\n 41\r\n2.0\r\n 41\r\n3.0\r\n"
                                              " 10\r\n0.0\r\n 20\r\n0.0\r\n 30\r\n0.0\r\n"));
  CHECK(Written(one).contains(" 30\r\n0.0\r\n 41\r\n2.0\r\n 41\r\n3.0\r\n"));
  CHECK(DxfIO::Read<Spline>(Written(one)) == one);
}

TEST_CASE("spline: fit points alone, the rest AutoCAD's to work out")
{
  const Spline one = DxfIO::Read<Spline>(Held(" 70\r\n  1064\r\n 71\r\n     3\r\n 11\r\n0.0\r\n 21\r\n0.0\r\n"
                                              " 11\r\n1.0\r\n 21\r\n1.0\r\n 11\r\n2.0\r\n 21\r\n0.0\r\n"));
  CHECK(one.Curve->Fits.size() == 3);
  CHECK(one.Curve->Knots.empty());
  CHECK(one.Curve->Poles.empty());
}

TEST_CASE("spline: tolerances a record leaves out are AutoCAD's")
{
  const Spline one = DxfIO::Read<Spline>(Held(" 71\r\n     3\r\n"));
  CHECK(one.Curve->Knotted == Knotting);
  CHECK(one.Curve->Framed == Framing);
}

TEST_CASE("spline: what AutoCAD takes and ignores kept, after the fields")
{
  const Spline one = DxfIO::Read<Spline>(Held(" 71\r\n     3\r\n 39\r\n1.0\r\n"));
  REQUIRE(one.Record.Base.Extra.size() == 1);
  CHECK(one.Record.Base.Extra.front().Code == 39);
}

TEST_CASE("spline: a new one cubic")
{
  CHECK(Spline{}.Curve->Degree == 3);
}

TEST_CASE("spline: a copy is its own, its box deep")
{
  Spline one{};
  one.Curve->Degree = 3;
  Spline two = one;
  two.Curve->Degree = 2;
  CHECK(one.Curve->Degree == 3);
}

TEST_CASE("spline: an entity stays the size of the small kinds")
{
  CHECK(sizeof(Spline) < 2 * sizeof(Record));
}
} // namespace DxfIO::Entities

namespace DxfIO::Entities
{
TEST_CASE("spline: a pole or a weight not finite fails the write")
{
  Spline one{};
  one.Curve->Poles = {Vector{.X = std::numeric_limits<double>::quiet_NaN()}, Vector{}};
  one.Curve->Weights = {1, 1};
  Fields::Sheet page{};
  CHECK_FALSE(Given(page.To, one));
  Spline heavy{};
  heavy.Curve->Poles = {Vector{}, Vector{}};
  heavy.Curve->Weights = {std::numeric_limits<double>::quiet_NaN(), 1};
  Fields::Sheet other{};
  CHECK_FALSE(Given(other.To, heavy));
}
} // namespace DxfIO::Entities
