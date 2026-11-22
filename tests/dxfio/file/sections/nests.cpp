#include "file/sections/nests.hpp"

#include "tags/tags.hpp"

#include <doctest/doctest.h>

#include <vector>

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
    CHECK(found->front().End->Name      == "ENDTAB");
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
    CHECK(found->front().End->Name            == "ENDBLK");
  }

  TEST_CASE("nests: a nest opened wrong is a failure")
  {
    const auto block = Tags::All("  0\nBLOCK\n  2\nbolt\n");
    REQUIRE(block);
    CHECK(Tables(*block)
          == std::unexpected{Failure{
              .Code = FailureCode::Damaged, .Line = 1, .What = "TABLE expected, found BLOCK"}});
  }

  TEST_CASE("nests: a block missing its ENDBLK closes at the next BLOCK, which is kept, told")
  {
    const auto all = Tags::All("  0\nBLOCK\n  2\nbolt\n  0\nCIRCLE\n"
                               "  0\nBLOCK\n  2\nnut\n  0\nLINE\n  0\nENDBLK\n");
    REQUIRE(all);
    std::vector<Issue> issues;
    const auto         found = Blocks(*all, &issues);
    REQUIRE(found);
    REQUIRE(found->size() == 2);
    CHECK(found->front().Records.size() == 1);
    CHECK_FALSE(found->front().End);
    CHECK(found->back().Records.size() == 1);
    CHECK(found->back().End->Name      == "ENDBLK");
    CHECK(issues
          == std::vector{Issue{.Code = IssueCode::Damaged,
                               .Line = 1,
                               .What = "ENDBLK of bolt missing, closed at the next BLOCK"}});
  }

  TEST_CASE("nests: the last nest missing its closer closes at the end of the section, told")
  {
    const auto all = Tags::All("  0\nTABLE\n 70\n1\n  0\nLAYER\n  2\n0\n");
    REQUIRE(all);
    std::vector<Issue> issues;
    const auto         found = Tables(*all, &issues);
    REQUIRE(found);
    REQUIRE(found->size() == 1);
    CHECK(found->front().Records.size() == 1);
    CHECK(issues
          == std::vector{
              Issue{.Code = IssueCode::Damaged,
                    .Line = 1,
                    .What = "ENDTAB of a TABLE missing, closed at the end of the section"}});
    CHECK(Tables(*all));
  }

  TEST_CASE("nests: an end record keeps the groups after its name")
  {
    const auto all = Tags::All("  0\nBLOCK\n  2\nbolt\n  0\nPOINT\n  0\nENDBLK\n  8\n0\n");
    REQUIRE(all);
    const auto found = Blocks(*all);
    REQUIRE(found);
    REQUIRE(found->size() == 1);
    CHECK(found->front().Records.size()   == 1);
    CHECK(found->front().End->Tags.size() == 2);
  }
} // namespace DxfIO::File::Sections
