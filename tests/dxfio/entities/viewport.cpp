#include "entities/shapes/viewport.hpp"

#include "../alike.hpp"
#include "../kind.hpp"
#include "../written.hpp"
#include "drawn.hpp"

#include "dxfio/load.hpp"
#include "dxfio/save.hpp"
#include "dxfio/tables/vport.hpp"

#include <array>
#include <string>
#include <string_view>
#include <variant>

namespace DxfIO::Entities
{
namespace
{
auto Marked(std::string_view self, Version release = Version::R2018) -> std::string
{
  return Drawn(self, release, "viewports");
}

auto Alikened(const std::string &record, Version release = Version::R2018) -> Viewport
{
  const Viewport out = DxfIO::Read<Viewport>(record, release);
  CHECK(out.Record.Base.Extra.empty());
  CHECK(Alike(Written(out, release), record));
  return out;
}

constexpr std::string_view Sparse = "  0\r\nVIEWPORT\r\n  5\r\n2A\r\n330\r\n1F\r\n100\r\nAcDbEntity\r\n"
                                    " 67\r\n     1\r\n  8\r\n0\r\n100\r\nAcDbViewport\r\n 10\r\n1.0\r\n"
                                    " 20\r\n2.0\r\n 30\r\n0.0\r\n 40\r\n3.0\r\n 41\r\n4.0\r\n"
                                    " 68\r\n     2\r\n 69\r\n     2\r\n";
} // namespace

TEST_CASE("viewport: the paper's own, number 1, first of its layout")
{
  const Viewport paper = Alikened(Marked("2DA"));
  CHECK(paper.Record.Paper);
  CHECK(paper.Port->Id == 1);
  CHECK(paper.Port->Status == 1);
  CHECK(paper.Port->Flags == 819232);
  CHECK(paper.Port->Style);
  CHECK(paper.Port->Rgb);
}

TEST_CASE("viewport: a floating one, its window and what it shows")
{
  const Viewport one = Alikened(Marked("2DE"));
  CHECK(one.Port->Id == 2);
  CHECK(one.Port->Width == 205.6);
  CHECK(one.Port->Height == 156);
  CHECK(one.Port->View == Vector{.X = 50, .Y = 25, .Z = 0});
  CHECK(one.Port->Extent == doctest::Approx(76.80079719085126));
  CHECK(one.Port->Direction == Up);
  CHECK(one.Port->Lens == 50);
  CHECK(one.Port->Zoom == 1000);
  CHECK(one.Port->Sheet.empty());
}

TEST_CASE("viewport: off, 0 in the stack and bit 131072; locked, bit 16384")
{
  const Viewport dark = Alikened(Marked("2E6"));
  CHECK(dark.Port->Status == 0);
  CHECK((dark.Port->Flags & 131072) != 0);
  CHECK((Alikened(Marked("2EA")).Port->Flags & 16384) != 0);
}

TEST_CASE("viewport: the stack apart from the number, as AutoCAD left it")
{
  const Viewport one = Alikened(Marked("2EE"));
  CHECK(one.Port->Id == 6);
  CHECK(one.Port->Status == 9);
}

TEST_CASE("viewport: its frozen layers on 341 at R2000, 331 from R2004")
{
  const std::string old = Marked("2EE", Version::R2000);
  CHECK(old.contains("341\r\n"));
  CHECK_FALSE(old.contains("331\r\n"));
  CHECK(Alikened(old, Version::R2000).Port->Frozen.size() == 2);
  const std::string late = Marked("2EE", Version::R2004);
  CHECK(late.contains("331\r\n"));
  CHECK(Alikened(late, Version::R2004).Port->Frozen.size() == 2);
  const Viewport now = Alikened(Marked("2EE"));
  CHECK(now.Port->Frozen.size() == 2);
  CHECK(now.Port->Frozen.front() != now.Port->Frozen.back());
}

TEST_CASE("viewport: 341 read at R2000 goes out as 331 at R2018")
{
  const Viewport old = DxfIO::Read<Viewport>(Marked("2EE", Version::R2000), Version::R2000);
  const std::string out = Written(old, Version::R2018);
  CHECK(out.contains("331\r\n"));
  CHECK_FALSE(out.contains("341\r\n"));
}

TEST_CASE("viewport: either code read at any release, the release's written")
{
  const std::string record = std::string{Sparse} + "331\r\n2B\r\n341\r\n2C\r\n";
  for (const Version release : {Version::R2000, Version::R2018})
  {
    CAPTURE(release);
    const Viewport taken = DxfIO::Read<Viewport>(record, release);
    CHECK(taken.Port->Frozen.size() == 2);
    CHECK(taken.Record.Base.Extra.empty());
  }
  const Viewport both = DxfIO::Read<Viewport>(record);
  CHECK(Written(both, Version::R2000).contains("341\r\n2B\r\n"));
  CHECK_FALSE(Written(both, Version::R2000).contains("331\r\n"));
  CHECK(Written(both, Version::R2018).contains("331\r\n2C\r\n"));
  CHECK_FALSE(Written(both, Version::R2018).contains("341\r\n"));
}

TEST_CASE("viewport: clipped by a boundary, a polygon's or a circle's")
{
  for (const std::string_view self : {"2F3", "2F8"})
  {
    const Viewport one = Alikened(Marked(self));
    CHECK(one.Port->Clip);
    CHECK((one.Port->Flags & 65536) != 0);
  }
}

TEST_CASE("viewport: shaded in plot, from R2004 and not at R2000")
{
  CHECK(Alikened(Marked("2FC")).Port->Shade == 2);
  CHECK_FALSE(Marked("2FC", Version::R2000).contains("170\r\n"));
}

TEST_CASE("viewport: snap, grid and major lines, its UCS named")
{
  const Viewport one = Alikened(Marked("300"));
  CHECK(one.Port->Snap.X == 2.5);
  CHECK(one.Port->Snap.Y == 5);
  CHECK(one.Port->Grid.X == 5);
  CHECK(one.Port->Grid.Y == 7.5);
  CHECK(one.Port->Base == Vector{.X = 1, .Y = 2, .Z = 0});
  CHECK(one.Port->Angle == doctest::Approx(28.64788975654116));
  CHECK(one.Port->Major == 3);
  CHECK(one.Port->Axes.Named);
  CHECK(one.Port->Axes.Origin.X == 7);
}

TEST_CASE("viewport: its visual style and lighting from R2007")
{
  const Viewport one = Alikened(Marked("304"));
  CHECK(one.Port->Render == 6);
  CHECK_FALSE(one.Port->Lit);
  CHECK(Alikened(Marked("2DE")).Port->Style != one.Port->Style);
  CHECK_FALSE(Marked("304", Version::R2004).contains("348\r\n"));
}

TEST_CASE("viewport: a view from a corner, and a perspective")
{
  const Viewport corner = Alikened(Marked("308"));
  CHECK(corner.Port->Direction == Vector{.X = 1, .Y = 1, .Z = 1});
  CHECK(corner.Port->Target == Vector{.X = 50, .Y = 25, .Z = 0});
  const Viewport deep = Alikened(Marked("30C"));
  CHECK((deep.Port->Flags & 1) != 0);
  CHECK(deep.Port->Direction.Z == 100);
}

TEST_CASE("viewport: a bare record takes what AutoCAD gives what is left out")
{
  const Viewport bare = DxfIO::Read<Viewport>(Sparse);
  CHECK(bare.Port->Flags == 32768);
  CHECK(bare.Port->Extent == -1);
  CHECK(bare.Port->Zoom == 100);
  CHECK(bare.Port->Lens == 0);
  CHECK(bare.Port->Direction == Up);
  CHECK(bare.Port->Major == 5);
  CHECK(bare.Port->Lit);
  CHECK(bare.Port->Ambient == 250);
  CHECK_FALSE(bare.Port->Rgb);
  CHECK_FALSE(bare.Port->Style);
  CHECK(bare.Record.Paper);
  const std::string out = Written(bare);
  CHECK_FALSE(out.contains("421\r\n"));
  CHECK_FALSE(out.contains("348\r\n"));
  CHECK(DxfIO::Read<Viewport>(out) == bare);
}

TEST_CASE("viewport: what no script makes, in its place, written back")
{
  const std::string record = std::string{Sparse} +
                             " 12\r\n0.0\r\n 22\r\n0.0\r\n 13\r\n0.0\r\n 23\r\n0.0\r\n"
                             " 14\r\n0.0\r\n 24\r\n0.0\r\n 15\r\n0.0\r\n 25\r\n0.0\r\n"
                             " 16\r\n0.0\r\n 26\r\n0.0\r\n 36\r\n1.0\r\n 17\r\n0.0\r\n 27\r\n0.0\r\n"
                             " 37\r\n0.0\r\n 42\r\n50.0\r\n 43\r\n0.0\r\n 44\r\n0.0\r\n 45\r\n1.0\r\n"
                             " 50\r\n0.0\r\n 51\r\n0.0\r\n 72\r\n  1000\r\n 90\r\n     32768\r\n"
                             "  1\r\nmonochrome.ctb\r\n281\r\n     0\r\n 71\r\n     1\r\n"
                             " 74\r\n     "
                             "0\r\n110\r\n0.0\r\n120\r\n0.0\r\n130\r\n0.0\r\n111\r\n1.0\r\n"
                             "121\r\n0.0\r\n131\r\n0.0\r\n112\r\n0.0\r\n122\r\n1.0\r\n132\r\n0.0\r\n"
                             " 79\r\n     0\r\n146\r\n0.0\r\n170\r\n     0\r\n 61\r\n     5\r\n"
                             "332\r\n2B\r\n333\r\n2C\r\n348\r\n2D\r\n292\r\n     1\r\n282\r\n     "
                             "1\r\n"
                             "141\r\n0.5\r\n142\r\n0.25\r\n 63\r\n     1\r\n421\r\n   3355443\r\n"
                             "431\r\nbook$color\r\n361\r\n2E\r\n";
  const Viewport taken = Alikened(record);
  CHECK(taken.Port->Sheet == "monochrome.ctb");
  CHECK(taken.Port->Background == Handle{.Value = 0x2B});
  CHECK(taken.Port->Plot == Handle{.Value = 0x2C});
  CHECK(taken.Port->Brightness == 0.5);
  CHECK(taken.Port->Contrast == 0.25);
  CHECK(taken.Port->Ambient == 1);
  CHECK(taken.Port->Book == "book$color");
  CHECK(taken.Port->Sun == Handle{.Value = 0x2E});
}

TEST_CASE("viewport: R12 holds the view in extended data, kept as it is")
{
  const Viewport old = DxfIO::Read<Viewport>(Marked("2DE", Version::R12), Version::R12);
  CHECK(old.Port->Width == 205.6);
  CHECK(old.Port->Id == 2);
  CHECK(old.Record.Base.Extended.size() == 1);
  CHECK(Written(old, Version::R12).contains("MVIEW"));
}

TEST_CASE("viewport: a copy is its own, its box deep")
{
  Viewport one{};
  one.Port->Id = 1;
  Viewport other = one;
  other.Port->Id = 2;
  CHECK(one.Port->Id == 1);
  CHECK_FALSE(one == other);
}

TEST_CASE("viewport: an entity stays the size of the small kinds")
{
  CHECK(sizeof(Viewport) < 2 * sizeof(Record));
}

TEST_CASE("viewport: every release alike")
{
  for (const Version release : Releases)
  {
    CAPTURE(release);
    CHECK(Alikened(Marked("2DE", release), release).Port->Id == 2);
  }
}

TEST_CASE("viewport: an R12 drawing's come back the same, blocks and all")
{
  const auto theirs = Load(Acad(Fixture("viewports", Version::R12)));
  REQUIRE(theirs);
  CHECK(theirs->Issues.empty());
  const auto ours = Tests::Written(theirs->Document, Options{.Release = Version::R12});
  REQUIRE(ours);
  const auto again = Load(*ours);
  REQUIRE(again);
  CHECK(again->Document.Entities == theirs->Document.Entities);
  CHECK(again->Document.Blocks == theirs->Document.Blocks);
}

TEST_CASE("viewport: its grid's major lines hide no table's spacing")
{
  using namespace DxfIO::Tables;
  CHECK(Vport{}.Snap.X == Snapunit);
}
} // namespace DxfIO::Entities
