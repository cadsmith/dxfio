#include "file/sections/nests.hpp"

#include "tags/tags.hpp"

#include <doctest/doctest.h>

namespace DxfIO::File::Sections
{
  TEST_CASE("nests: a table runs from its TABLE to its ENDTAB")
  {
    const auto all = Tags::All("  0\nTABLE\n  2\nLAYER\n  0\nLAYER\n  2\n0\n"
                               "  0\nENDTAB\n  0\nTABLE\n  2\nLTYPE\n"
                               "  0\nENDTAB\n");
    REQUIRE(all);
    const auto found = Tables(*all);
    REQUIRE(found);
    REQUIRE(found->size()          == 2);
    CHECK(found->front().Head.Name == "TABLE");
    CHECK(found->front().Records.size() == 1);
    CHECK(found->front().End.Name       == "ENDTAB");
    CHECK(found->back().Records.empty());
  }

  TEST_CASE("nests: a block runs from its BLOCK to its ENDBLK")
  {
    const auto all = Tags::All("  0\nBLOCK\n  2\nbolt\n  0\nCIRCLE\n 40\n1.0\n  0\nENDBLK\n");
    REQUIRE(all);
    const auto found = Blocks(*all);
    REQUIRE(found);
    REQUIRE(found->size() == 1);
    CHECK(found->front().Records.front().Name == "CIRCLE");
    CHECK(found->front().End.Name             == "ENDBLK");
  }

  TEST_CASE("nests: a nest opened wrong or never closed is a failure")
  {
    const auto block = Tags::All("  0\nBLOCK\n  2\nbolt\n");
    REQUIRE(block);
    CHECK(Tables(*block) == std::unexpected{Failure{1, "TABLE expected, found BLOCK"}});
    CHECK(Blocks(*block) == std::unexpected{Failure{3, "ENDBLK missing"}});
  }

  TEST_CASE("nests: an end record keeps the groups after its name")
  {
    const auto all = Tags::All("  0\nBLOCK\n  2\nbolt\n  0\nPOINT\n  0\nENDBLK\n  8\n0\n");
    REQUIRE(all);
    const auto found = Blocks(*all);
    REQUIRE(found);
    REQUIRE(found->size() == 1);
    CHECK(found->front().Records.size()  == 1);
    CHECK(found->front().End.Tags.size() == 2);
  }
} // namespace DxfIO::File::Sections
