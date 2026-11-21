#include "empty/blocks.hpp"

#include "../autocad.hpp"
#include "../disk.hpp"
#include "../fields/fixture.hpp"
#include "../releases.hpp"
#include "blocks/io.hpp"
#include "empty/roots.hpp"
#include "empty/space.hpp"

#include "dxfio/version.hpp"

#include <doctest/doctest.h>

#include <array>
#include <string>
#include <utility>

namespace DxfIO::Empty
{
  TEST_CASE("block: named, owned, in paper space, on handles of its own")
  {
    Seed                     handles{Handle{.Value = 0x40}};
    const Handle             record{.Value = 0x31};
    const Blocks::Definition sheet =
        Block("*Paper_Space7", Space{.Record = record, .Paper = true}, handles);
    CHECK(sheet.Block.Name             == "*Paper_Space7");
    CHECK(sheet.Block.Record.Base.Self == Handle{.Value = 0x40});
    CHECK(sheet.EndBlk.Record.Base.Self  == Handle{.Value = 0x41});
    CHECK(sheet.Block.Record.Base.Owner  == record);
    CHECK(sheet.EndBlk.Record.Base.Owner == record);
    CHECK(sheet.Block.Record.Paper);
    CHECK(sheet.EndBlk.Record.Paper);
    CHECK(sheet.Entities.empty());
    CHECK_FALSE(Block("*Model_Space", Space{.Record = record}, handles).EndBlk.Record.Paper);
  }

  TEST_CASE("blocks: a new drawing's two spaces, each of its own record")
  {
    Seed                                  handles;
    const Roots                           ids = Taken(handles);
    const std::vector<Blocks::Definition> all = Blocks(ids, handles);
    REQUIRE(all.size()           == 2);
    CHECK(all.front().Block.Name == "*Model_Space");
    CHECK(all.front().Block.Record.Base.Owner  == ids.Records.Model);
    CHECK(all.front().EndBlk.Record.Base.Owner == ids.Records.Model);
    CHECK_FALSE(all.front().Block.Record.Paper);
    CHECK(all.back().Block.Name == "*Paper_Space");
    CHECK(all.back().EndBlk.Record.Base.Owner == ids.Records.Paper);
    CHECK(all.back().EndBlk.Record.Paper);
    CHECK(all.front().EndBlk.Record.Base.Self != all.back().Block.Record.Base.Self);
  }

  TEST_CASE("blocks: written as AutoCAD writes its empty drawing's, from R2000")
  {
    for (const Version release : Releases)
    {
      const std::string source = Acad(Fixture("empty", release));
      CAPTURE(release);
      Seed          handles;
      const Roots   ids = Taken(handles);
      Fields::Sheet page{release};
      REQUIRE(Blocks::Write(page.To, Blocks(ids, handles)));
      const std::string theirs = Masked(Autocad(source, "20"))
                               + Masked(Autocad(source, "21"))
                               + Masked(Autocad(source, "D4"))
                               + Masked(Autocad(source, "D5"));
      CHECK(Masked(page.Out) == theirs);
    }
  }
} // namespace DxfIO::Empty
