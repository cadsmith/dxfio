#include "entities/shapes/text.hpp"

#include "../alike.hpp"
#include "../kind.hpp"
#include "../written.hpp"
#include "drawn.hpp"
#include "fields/read.hpp"
#include "file/cursor.hpp"
#include "tags/tags.hpp"
#include "unknown/io.hpp"

#include "dxfio/load.hpp"
#include "dxfio/save.hpp"

#include <array>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace DxfIO::Entities
{
namespace
{
auto Marked(std::string_view self, Version release = Version::R2018) -> std::string
{
  return Drawn(self, release, "texts");
}

auto Alikened(const std::string &record, Version release = Version::R2018) -> Text
{
  const Text out = DxfIO::Read<Text>(record, release);
  CHECK(out.Record.Base.Extra.empty());
  CHECK(Alike(Written(out, release), record));
  return out;
}

auto Wrong(std::string_view record) -> Text
{
  const auto all = Tags::All(record);
  REQUIRE(all);
  File::Cursor in{*all};
  const auto one = in.Take();
  REQUIRE(one);
  const Fields::Reading with{.Release = Version::R2018, .Text = Encoding::Utf8()};
  return Taken<Text>(*one, with);
}

auto Settled(std::string_view groups) -> Text
{
  const Text first = Wrong(std::string{"  0\nTEXT\n  5\n2A\n"} + std::string{groups});
  const std::string once = Written(first);
  const Text again = Wrong(once);
  CHECK(again == first);
  CHECK(Written(again) == once);
  return first;
}

struct Aligned
{
  std::string_view Self;
  int Horizontal{};
  int Vertical{};
};
} // namespace

TEST_CASE("text: a plain one, its point, height and value")
{
  const Text taken = Replayed<Text>(Marked("2D1"));
  CHECK(taken.Lettering.Position == Vector{});
  CHECK(taken.Lettering.Height == 2);
  CHECK(taken.Lettering.Value == "left");
  CHECK_FALSE(taken.Lettering.Alignment);
  CHECK(taken.Vertical == 0);
}

TEST_CASE("text: every alignment, 11 given but for the plain one")
{
  constexpr std::array all{
      Aligned{.Self = "2D2", .Horizontal = 1, .Vertical = 0}, Aligned{.Self = "2D3", .Horizontal = 2, .Vertical = 0},
      Aligned{.Self = "2D4", .Horizontal = 4, .Vertical = 0}, Aligned{.Self = "2D5", .Horizontal = 3, .Vertical = 0},
      Aligned{.Self = "2D6", .Horizontal = 5, .Vertical = 0}, Aligned{.Self = "2D7", .Horizontal = 0, .Vertical = 3},
      Aligned{.Self = "2D8", .Horizontal = 1, .Vertical = 3}, Aligned{.Self = "2D9", .Horizontal = 2, .Vertical = 3},
      Aligned{.Self = "2DA", .Horizontal = 0, .Vertical = 2}, Aligned{.Self = "2DB", .Horizontal = 1, .Vertical = 2},
      Aligned{.Self = "2DC", .Horizontal = 2, .Vertical = 2}, Aligned{.Self = "2DD", .Horizontal = 0, .Vertical = 1},
      Aligned{.Self = "2DE", .Horizontal = 1, .Vertical = 1}, Aligned{.Self = "2DF", .Horizontal = 2, .Vertical = 1},
  };
  for (const Aligned &one : all)
  {
    CAPTURE(one.Self);
    const Text taken = Alikened(Marked(one.Self));
    CHECK(taken.Lettering.Horizontal == one.Horizontal);
    CHECK(taken.Vertical == one.Vertical);
    CHECK(taken.Lettering.Alignment);
  }
}

TEST_CASE("text: aligned and fit keep what AutoCAD worked out")
{
  const Text between = Alikened(Marked("2D5"));
  CHECK(between.Lettering.Position == Vector{.X = 0, .Y = 20, .Z = 0});
  CHECK(between.Lettering.Alignment == Vector{.X = 30, .Y = 25, .Z = 0});
  CHECK(between.Lettering.Height == doctest::Approx(6.750438962465699));
  CHECK(between.Lettering.Rotation == doctest::Approx(9.462322208025616));
  const Text fit = Alikened(Marked("2D6"));
  CHECK(fit.Lettering.Height == 2);
  CHECK(fit.Lettering.Width == doctest::Approx(13.99455409513056));
}

TEST_CASE("text: rotated, styled, leaning, mirrored both ways")
{
  CHECK(Alikened(Marked("2E0")).Lettering.Rotation == 30);
  CHECK(Alikened(Marked("2E1")).Lettering.Style == "tall");
  const Text turned = Alikened(Marked("2E2"));
  CHECK(turned.Lettering.Style == "turned");
  CHECK(turned.Lettering.Width == 0.8);
  CHECK(turned.Lettering.Oblique == 15);
  CHECK(turned.Lettering.Generation == 6);
  CHECK(Alikened(Marked("2E8")).Lettering.Generation == 4);
}

TEST_CASE("text: thick, on a layer of its own, with what an entity holds")
{
  const Text made = Alikened(Marked("2E7"));
  CHECK(made.Lettering.Thickness == 3);
  CHECK(made.Lettering.Width == 0.75);
  CHECK(made.Lettering.Generation == 2);
  CHECK(made.Record.Layer == "notes");
  CHECK(made.Record.Linetype == "DASHED");
  CHECK(made.Record.Color == 1);
}

TEST_CASE("text: a side of its own given without the other")
{
  const Text up = Alikened(Marked("2E9"));
  CHECK(up.Lettering.Horizontal == 0);
  CHECK(up.Vertical == 2);
  const Text across = Alikened(Marked("2EA"));
  CHECK(across.Lettering.Horizontal == 1);
  CHECK(across.Vertical == 0);
}

TEST_CASE("text: on another plane, 210 before the second marker")
{
  const Text behind = Alikened(Marked("2EB"));
  CHECK(behind.Lettering.Extrusion.Z == -1);
  CHECK(behind.Lettering.Alignment == Vector{.X = 4, .Y = 5, .Z = 3});
  CHECK(behind.Vertical == 1);
}

TEST_CASE("text: empty, longer than a group once was, escaped as written")
{
  CHECK(Alikened(Marked("2EC")).Lettering.Value.empty());
  CHECK(Alikened(Marked("2ED")).Lettering.Value.size() == 300);
  CHECK(Alikened(Marked("2ED", Version::R12), Version::R12).Lettering.Value.size() == 299);
  CHECK(Alikened(Marked("2E3")).Lettering.Value.starts_with("%%uunder%%u"));
}

TEST_CASE("text: in paper space")
{
  CHECK(Alikened(Marked("302")).Record.Paper);
}

TEST_CASE("text: every release alike, R12 with no marker")
{
  for (const Version release : Releases)
  {
    CAPTURE(release);
    CHECK(Alikened(Marked("2DB", release), release).Vertical == 2);
  }
  const Text old = Alikened(Marked("2DB", Version::R12), Version::R12);
  CHECK(old.Vertical == 2);
  CHECK(old.Lettering.Horizontal == 1);
}

TEST_CASE("text: its two markers read as one, as every codec reads them")
{
  const std::string_view head = "  0\r\nTEXT\r\n  5\r\n2A\r\n330\r\n1F\r\n100\r\nAcDbEntity\r\n"
                                "  8\r\n0\r\n";
  const std::string_view word = " 40\r\n2.0\r\n  1\r\nx\r\n";
  const auto joined = [&](std::string_view rest, std::string_view after = {}) -> DxfIO::Entities::Text
  {
    return DxfIO::Read<Text>(std::string{head} + std::string{rest} + std::string{word} + std::string{after});
  };
  const std::string_view one = "100\r\nAcDbText\r\n";
  const std::string both = std::string{one} + std::string{word} + std::string{one} + " 72\r\n     1\r\n";
  CHECK(joined(one, " 73\r\n     2\r\n").Vertical == 2);
  CHECK(joined({}, std::string{one} + " 73\r\n     3\r\n").Vertical == 3);
  CHECK(joined({}, std::string{one} + " 73\r\n     3\r\n").Lettering.Height == 2);
  const Text past = DxfIO::Read<Text>(std::string{head} + both);
  CHECK(past.Lettering.Horizontal == 1);
  CHECK(past.Lettering.Value == "x");
  const Text three = joined(one, std::string{one} + std::string{one} + " 73\r\n     1\r\n");
  CHECK(three.Vertical == 1);
  CHECK(three.Record.Base.Extra.empty());
}

TEST_CASE("text: however wrong, what is read is what the next load reads")
{
  const Text between = Settled("100\nAcDbText\n 40\n2.0\n  1\nx\n100\nAcDbEntity\n  8\n0\n"
                               "100\nAcDbText\n 73\n     1\n 40\n9.0\n");
  CHECK(between.Lettering.Height == 9);
  CHECK(between.Vertical == 1);
  const Text stray = Settled("100\nAcDbEntity\n  8\n0\n100\nAcDbText\n 40\n2.0\n  1\nx\n"
                             "100\nAcDbText\n 73\n     1\n100\nAcDbThing\n100\nAcDbText\n");
  CHECK(stray.Vertical == 1);
  CHECK(stray.Record.Base.Extra.size() == 1);
  const Text scattered = Settled("100\nAcDbEntity\n  8\n0\n100\nAcDbText\n 40\n2.0\n  1\nx\n"
                                 "100\nAcDbThing\n100\nAcDbText\n 73\n     1\n100\nAcDbThing\n"
                                 "100\nAcDbText\n 39\n3.0\n");
  CHECK(scattered.Vertical == 1);
  CHECK(scattered.Lettering.Thickness == 3);
  const Text rotated = Settled("100\nAcDbText\n 40\n2.0\n  1\nx\n100\nAcDbEntity\n  8\n0\n"
                               "100\nAcDbText\n 50\n30.0\n");
  CHECK(rotated.Lettering.Rotation == 30);
  CHECK(Settled("100\nAcDbEntity\n  8\n0\n 40\n2.0\n  1\nx\n"
                " 72\n99999999999\n")
            .Record.Base.Extra.empty());
  CHECK(Settled("100\nAcDbEntity\n  8\n0\n100\nAcDbText\n 40\n2.0\n"
                "  1\nx\n100\nAcDbThing\n 90\n1\n")
            .Record.Base.Extra.size() == 2);
}

TEST_CASE("text: a number too large for its group is left out before the record, told")
{
  std::vector<Issue> issues;
  const auto all = Tags::All("100\nAcDbEntity\n  8\n0\n100\nAcDbText\n 40\n2.0\n  1\nx\n"
                             " 72\n99999999999\n 71\n99999999999\n100\nAcDbText\n",
                             &issues);
  REQUIRE(all);
  Fields::Walk in{*all, Fields::Reading{.Issues = &issues}};
  Text taken{};
  Fields::Read(taken, in, Shapes<Text>);
  CHECK(in.Kept().empty());
  CHECK(issues.size() == 2);
  CHECK(taken.Lettering.Horizontal == 0);
}

TEST_CASE("text: another class's marker ends it, what follows kept")
{
  const Text kept = DxfIO::Read<Text>("  0\r\nTEXT\r\n  5\r\n2A\r\n100\r\nAcDbEntity\r\n  8\r\n0\r\n"
                                      "100\r\nAcDbText\r\n 40\r\n2.0\r\n  1\r\nx\r\n100\r\nAcDbText\r\n"
                                      " 73\r\n     1\r\n100\r\nAcDbThing\r\n 40\r\n9.0\r\n");
  CHECK(kept.Lettering.Height == 2);
  CHECK(kept.Vertical == 1);
  CHECK(kept.Record.Base.Extra.size() == 2);
}

TEST_CASE("text: an R12 drawing's come back the same, blocks and all")
{
  const auto theirs = Load(Acad(Fixture("texts", Version::R12)));
  REQUIRE(theirs);
  CHECK(theirs->Issues.empty());
  const auto ours = Tests::Written(theirs->Document, Options{.Release = Version::R12});
  REQUIRE(ours);
  const auto again = Load(*ours);
  REQUIRE(again);
  CHECK(again->Document.Entities == theirs->Document.Entities);
  CHECK(again->Document.Blocks == theirs->Document.Blocks);
}
} // namespace DxfIO::Entities
