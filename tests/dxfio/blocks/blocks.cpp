#include "../fields/fixture.hpp"
#include "blocks/io.hpp"
#include "file/sections/nests.hpp"
#include "tags/tags.hpp"

#include <doctest/doctest.h>

#include <string_view>
#include <variant>

namespace DxfIO::Blocks
{
  TEST_CASE("blocks: a block's BLOCK, entities and ENDBLK, written as read")
  {
    const std::string_view text = "  0\r\nBLOCK\r\n  5\r\n20\r\n330\r\n1F\r\n100\r\nAcDbEntity\r\n"
                                  "  8\r\n0\r\n100\r\nAcDbBlockBegin\r\n  2\r\nbolt\r\n"
                                  " 70\r\n     0\r\n 10\r\n0.0\r\n 20\r\n0.0\r\n 30\r\n0.0\r\n"
                                  "  3\r\nbolt\r\n  1\r\n\r\n"
                                  "  0\r\nCIRCLE\r\n  5\r\n21\r\n330\r\n1F\r\n100\r\nAcDbEntity\r\n"
                                  "  8\r\n0\r\n100\r\nAcDbCircle\r\n 10\r\n0.0\r\n 20\r\n0.0\r\n"
                                  " 30\r\n0.0\r\n 40\r\n1.0\r\n"
                                  "  0\r\nENDBLK\r\n  5\r\n22\r\n330\r\n1F\r\n100\r\nAcDbEntity\r\n"
                                  "  8\r\n0\r\n100\r\nAcDbBlockEnd\r\n";
    const auto             all  = Tags::All(text);
    REQUIRE(all);
    const auto nests = File::Sections::Blocks(*all);
    REQUIRE(nests);
    std::vector<Issue>       issues;
    const std::vector<Block> taken = Read(*nests, Fields::Reading{.Issues = &issues});
    REQUIRE(taken.size()           == 1);
    CHECK(taken.front().Begin.Name == "bolt");
    CHECK(std::holds_alternative<Entities::Circle>(taken.front().Entities.front()));
    CHECK(taken.front().End.Record.Base.Self == Handle{0x22});
    Fields::Sheet page;
    REQUIRE(Write(page.To, taken));
    CHECK(page.Out == text);
  }
} // namespace DxfIO::Blocks
