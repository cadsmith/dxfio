#include "objects/shapes/sortents/table.hpp"

#include "../../autocad.hpp"
#include "../../disk.hpp"
#include "../../kind.hpp"
#include "../../releases.hpp"

#include <array>
#include <string>
#include <string_view>

namespace DxfIO::Objects::Sortents
{
  TEST_CASE("table: AutoCAD's, its pairs as they came, its 5s no handle of it")
  {
    for (const Version release : std::array{Version::R2000, Version::R2018})
    {
      CAPTURE(release);
      const std::string   text  = Autocad(Acad(Fixture("sorted", release)), "2CF");
      const SortEntsTable taken = Read<SortEntsTable>(text, release);
      CHECK(taken.Base.Self        == Handle{0x2cf});
      CHECK(taken.Base.Owner       == Handle{0x1ce});
      CHECK(taken.Block            == Handle{0x1f});
      REQUIRE(taken.Entries.size() == 4);
      CHECK(taken.Entries.front().Entity == Handle{0x2cc});
      CHECK(taken.Entries.front().Key    == Handle{0x2ce});
      CHECK(taken.Entries.back().Key     == Handle{0x2d2});
      CHECK(taken.Base.Extra.empty());
      CHECK(Written(taken, release) == text);
    }
  }

  TEST_CASE("table: a sort handle of 0 kept, a pair without one left without")
  {
    constexpr std::string_view text  = "  0\r\nSORTENTSTABLE\r\n  5\r\n2CF\r\n"
                                       "330\r\n1CE\r\n100\r\nAcDbSortentsTable\r\n"
                                       "330\r\n1F\r\n331\r\n2CC\r\n  5\r\n0\r\n"
                                       "331\r\n2CD\r\n331\r\n2CE\r\n  5\r\n2CC\r\n";
    const SortEntsTable        taken = Read<SortEntsTable>(text);
    REQUIRE(taken.Entries.size()  == 3);
    CHECK(taken.Entries.at(0).Key == Handle{});
    CHECK_FALSE(taken.Entries.at(1).Key);
    CHECK(taken.Entries.at(2).Key == Handle{0x2cc});
    CHECK(Written(taken)          == text);
  }

  TEST_CASE("table: below R13 its groups as they came, no marker to fence a 5")
  {
    constexpr std::string_view text  = "  0\r\nSORTENTSTABLE\r\n100\r\n\r\n"
                                       "331\r\nC\r\n  5\r\n2\r\n";
    const SortEntsTable        taken = Read<SortEntsTable>(text, Version::R12);
    CHECK(taken.Entries.empty());
    CHECK_FALSE(taken.Base.Self);
    CHECK(taken.Base.Extra.size()      == 3);
    CHECK(Written(taken, Version::R12) == text);
  }
} // namespace DxfIO::Objects::Sortents
