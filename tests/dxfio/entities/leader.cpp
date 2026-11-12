#include "entities/shapes/leader.hpp"

#include "../alike.hpp"
#include "../kind.hpp"
#include "../releases.hpp"
#include "drawn.hpp"
#include "entities/io.hpp"

#include <string>
#include <string_view>

namespace DxfIO::Entities
{
  namespace
  {
    auto Marked(std::string_view self, Version release = Version::R2018) -> std::string
    {
      return Drawn(self, release, "leaders");
    }

    auto Alikened(const std::string &record, Version release = Version::R2018) -> Leader
    {
      const Leader out = DxfIO::Read<Leader>(record, release);
      CHECK(out.Record.Base.Extra.empty());
      CHECK(Alike(Written(out, release), record));
      return out;
    }

    auto Held(std::string_view body) -> std::string
    {
      return "  0\r\nLEADER\r\n  5\r\n2A\r\n330\r\n1F\r\n100\r\nAcDbEntity\r\n"
             "  8\r\n0\r\n100\r\nAcDbLeader\r\n  3\r\nStandard\r\n"
           + std::string{body};
    }
  } // namespace

  TEST_CASE("leader: its text beside it, its annotation by handle")
  {
    const Leader one = Alikened(Marked("2CD"));
    CHECK(one.Lead->Style      == "ISO-25");
    CHECK(one.Lead->Annotation == 0);
    CHECK(one.Lead->Direction  == 0);
    CHECK(one.Lead->Height);
    CHECK(one.Lead->Points.size() == 3);
    CHECK(one.Lead->Note != Handle{});
  }

  TEST_CASE("leader: a bare one, what a new one holds left out")
  {
    const Leader one = Alikened(Marked("2D0"));
    CHECK(one.Lead->Arrow      == 1);
    CHECK(one.Lead->Path       == 0);
    CHECK(one.Lead->Annotation == Unannotated);
    CHECK(one.Lead->Direction  == 1);
    CHECK_FALSE(one.Lead->Height);
    CHECK(one.Lead->Note == Handle{});
  }

  TEST_CASE("leader: a spline path, no arrow")
  {
    CHECK(Alikened(Marked("2D1")).Lead->Path  == 1);
    CHECK(Alikened(Marked("2D2")).Lead->Arrow == 0);
    const Leader both = Alikened(Marked("2D4"));
    CHECK(both.Lead->Arrow == 0);
    CHECK(both.Lead->Path  == 1);
  }

  TEST_CASE("leader: its normal given, on paper")
  {
    CHECK(Alikened(Marked("2D5")).Lead->Normal == Vector{.Z = -1});
    CHECK(Alikened(Marked("2E6")).Record.Paper);
  }

  TEST_CASE("leader: every release from R2000 alike")
  {
    for (const Version release : Releases)
    {
      CAPTURE(release);
      CHECK(Alikened(Marked("2CD", release), release).Lead->Points.size() == 3);
    }
  }

  TEST_CASE("leader: the count ignored, the points its own groups")
  {
    const Leader one =
        DxfIO::Read<Leader>(Held(" 76\r\n     9\r\n 10\r\n1.0\r\n 20\r\n2.0\r\n 30\r\n0.0\r\n"));
    CHECK(one.Lead->Points.size() == 1);
    CHECK(Written(one).contains(" 76\r\n     1\r\n"));
  }

  TEST_CASE("leader: its offsets and horizontal in AutoCAD's order")
  {
    const std::string text = Held(" 76\r\n     1\r\n 10\r\n1.0\r\n 20\r\n2.0\r\n 30\r\n0.0\r\n"
                                  "210\r\n0.0\r\n220\r\n0.0\r\n230\r\n-1.0\r\n"
                                  "211\r\n0.0\r\n221\r\n1.0\r\n231\r\n0.0\r\n"
                                  "212\r\n1.0\r\n222\r\n1.0\r\n232\r\n0.0\r\n"
                                  "213\r\n2.0\r\n223\r\n1.0\r\n233\r\n0.0\r\n");
    const Leader      one  = Alikened(text);
    CHECK(one.Lead->Horizontal == Vector{.Y = 1});
    CHECK(one.Lead->Block      == Vector{.X = 1, .Y = 1});
  }

  TEST_CASE("leader: a color for its line, after its points")
  {
    const Leader one = Alikened(Held(" 76\r\n     1\r\n 10\r\n1.0\r\n 20\r\n2.0\r\n 30\r\n0.0\r\n"
                                     " 77\r\n     3\r\n"));
    CHECK(one.Lead->Color == 3);
  }

  TEST_CASE("leader: a group AutoCAD ignores kept after the fields")
  {
    const Leader one = DxfIO::Read<Leader>(Held(" 90\r\n     1\r\n"));
    REQUIRE(one.Record.Base.Extra.size()     == 1);
    CHECK(one.Record.Base.Extra.front().Code == 90);
  }

  TEST_CASE("leader: named as DXF names it, a copy its own")
  {
    CHECK(Named(Entity{Leader{}}) == "LEADER");
    Leader one{};
    one.Lead->Hook = 1;
    Leader two     = one;
    two.Lead->Hook = 0;
    CHECK(one.Lead->Hook == 1);
    CHECK(sizeof(Leader) < 2 * sizeof(Record));
  }

  TEST_CASE("leader: its style a symbol, as a layer is")
  {
    CHECK(std::is_same_v<decltype(Lead::Style), Symbol>);
  }
} // namespace DxfIO::Entities
