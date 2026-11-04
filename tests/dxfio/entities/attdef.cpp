#include "entities/shapes/attdef.hpp"

#include "../alike.hpp"
#include "../kind.hpp"
#include "../written.hpp"
#include "drawn.hpp"

#include "dxfio/entities/text.hpp"
#include "dxfio/load.hpp"
#include "dxfio/save.hpp"

#include <array>
#include <string>
#include <string_view>

namespace DxfIO::Entities
{
namespace
{
auto Marked(std::string_view self, Version release = Version::R2018) -> std::string
{
  return Drawn(self, release, "attdefs");
}

auto Alikened(const std::string &record, Version release = Version::R2018) -> Attdef
{
  const Attdef out = DxfIO::Read<Attdef>(record, release);
  CHECK(out.Record.Base.Extra.empty());
  CHECK(Alike(Written(out, release), record));
  return out;
}

struct Flagged
{
  std::string_view Self;
  int Flags{};
};

constexpr std::string_view Opening = "  0\r\nATTDEF\r\n  5\r\n2A\r\n330\r\n1F\r\n100\r\nAcDbEntity\r\n  "
                                     "8\r\n0\r\n100\r\nAcDbText\r\n 10\r\n0.0\r\n 20\r\n0.0\r\n 30\r\n0.0\r\n"
                                     " 40\r\n1.0\r\n  1\r\nx\r\n100\r\nAcDbAttributeDefinition\r\n";

auto Joined(std::string_view rest) -> std::string
{
  return std::string{Opening} + std::string{rest};
}
} // namespace

TEST_CASE("attdef: a plain one, its default, prompt and tag")
{
  const Attdef taken = Replayed<Attdef>(Marked("2D0"));
  CHECK(taken.Lettering->Value == "plain");
  CHECK(taken.Lettering->Height == 1);
  CHECK(taken.Prompt == "Prompt");
  CHECK(taken.Tag == "PLAIN");
  CHECK(taken.Flags == 0);
  CHECK(taken.Revision == 0);
  CHECK_FALSE(taken.Locked);
}

TEST_CASE("attdef: each flag, and two at once")
{
  constexpr std::array all{
      Flagged{.Self = "2D1", .Flags = 1}, Flagged{.Self = "2D2", .Flags = 2}, Flagged{.Self = "2D3", .Flags = 4},
      Flagged{.Self = "2D4", .Flags = 8}, Flagged{.Self = "2E0", .Flags = 5},
  };
  for (const Flagged &one : all)
  {
    CAPTURE(one.Self);
    CHECK(Alikened(Marked(one.Self)).Flags == one.Flags);
  }
}

TEST_CASE("attdef: no prompt, no default, each written all the same")
{
  CHECK(Alikened(Marked("2D2")).Prompt.empty());
  const Attdef bare = Alikened(Marked("2D5"));
  CHECK(bare.Lettering->Value.empty());
  CHECK(bare.Prompt == "Empty");
}

TEST_CASE("attdef: aligned, 74 past its flags, its 11 given")
{
  const Attdef right = Alikened(Marked("2D8"));
  CHECK(right.Lettering->Horizontal == 2);
  CHECK(right.Vertical == 3);
  CHECK(right.Lettering->Alignment == Vector{.X = 60, .Y = 5, .Z = 0});
  const Attdef left = Alikened(Marked("2D9"));
  CHECK(left.Lettering->Horizontal == 0);
  CHECK(left.Vertical == 2);
  CHECK(Alikened(Marked("2DA")).Lettering->Horizontal == 3);
  CHECK(Alikened(Marked("2DB")).Lettering->Horizontal == 5);
}

TEST_CASE("attdef: what an entity and a text hold, on another plane too")
{
  const Attdef made = Alikened(Marked("2DC"));
  CHECK(made.Lettering->Thickness == 3);
  CHECK(made.Lettering->Style == "tall");
  CHECK(made.Record.Layer == "notes");
  CHECK(made.Record.Color == 1);
  const Attdef behind = Alikened(Marked("2DD"));
  CHECK(behind.Lettering->Extrusion.Z == -1);
  CHECK(behind.Vertical == 1);
}

TEST_CASE("attdef: locked, on the last 280 from R2010 and the one at R2007")
{
  const Attdef late = Alikened(Marked("2DF"));
  CHECK(late.Locked);
  CHECK(late.Revision == 0);
  const Attdef early = Alikened(Marked("2DF", Version::R2007), Version::R2007);
  CHECK(early.Locked);
}

TEST_CASE("attdef: locked at R2004 in extended data, kept as such")
{
  const Attdef old = DxfIO::Read<Attdef>(Marked("2DF", Version::R2004), Version::R2004);
  CHECK_FALSE(old.Locked);
  CHECK(old.Record.Base.Extended.size() == 1);
  CHECK(Alike(Written(old, Version::R2004), Marked("2DF", Version::R2004)));
}

TEST_CASE("attdef: in paper space")
{
  CHECK(Alikened(Marked("2F2")).Record.Paper);
}

TEST_CASE("attdef: multi-line, what follows the lock kept and written back")
{
  const std::string record = Marked("2E1");
  const Attdef many = DxfIO::Read<Attdef>(record);
  CHECK(many.Tag == "MULTI");
  CHECK(many.Vertical == 3);
  CHECK_FALSE(many.Locked);
  CHECK_FALSE(many.Record.Base.Extra.empty());
  CHECK(Alike(Written(many), record));
}

TEST_CASE("attdef: every release alike, R12 with no marker")
{
  for (const Version release : Releases)
  {
    CAPTURE(release);
    CHECK(Alikened(Marked("2D8", release), release).Vertical == 3);
  }
  const Attdef old = Alikened(Marked("2D8", Version::R12), Version::R12);
  CHECK(old.Vertical == 3);
  CHECK(old.Prompt == "Prompt");
}

TEST_CASE("attdef: its 280s as ATTRIB's, the first its version, a lone a lock")
{
  const Attdef both = Replayed<Attdef>(Joined("280\r\n     0\r\n  3\r\np\r\n  2\r\nT\r\n 70\r\n     0\r\n"
                                              "280\r\n     1\r\n"));
  CHECK(both.Locked);
  const Attdef lone = DxfIO::Read<Attdef>(Joined("  3\r\np\r\n  2\r\nT\r\n 70\r\n     0\r\n280\r\n     1\r\n"));
  CHECK(lone.Locked);
  CHECK(lone.Revision == 0);
  CHECK(Written(lone).contains("280\r\n     0\r\n  3\r\np\r\n"));
}

TEST_CASE("attdef: a first 280 not 0 kept as the version, written back")
{
  const Attdef taken = Replayed<Attdef>(Joined("280\r\n     2\r\n  3\r\np\r\n  2\r\nT\r\n 70\r\n     0\r\n"
                                               "280\r\n     1\r\n"));
  CHECK(taken.Revision == 2);
  CHECK(taken.Locked);
}

TEST_CASE("attdef: a field length after its flags, where the table puts it")
{
  const Attdef taken = Replayed<Attdef>(Joined("280\r\n     0\r\n  3\r\np\r\n  2\r\nT\r\n 70\r\n     0\r\n"
                                               " 73\r\n     5\r\n280\r\n     0\r\n"));
  CHECK(taken.Length == 5);
}

TEST_CASE("attdef: no larger than a text, its lettering boxed, and a copy its own")
{
  CHECK(sizeof(Attdef) <= sizeof(Text));
  Attdef one{};
  one.Lettering->Value = "one";
  Attdef two = one;
  two.Lettering->Value = "two";
  CHECK(one.Lettering->Value == "one");
}

TEST_CASE("attdef: an R12 drawing's come back the same, blocks and all")
{
  const auto theirs = Load(Acad(Fixture("attdefs", Version::R12)));
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
