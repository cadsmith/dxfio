#include "dxfio/raw.hpp"

#include "fields/fixture.hpp"
#include "file/cursor.hpp"
#include "raw/io.hpp"
#include "tags/tags.hpp"

#include <doctest/doctest.h>

#include <string_view>

namespace DxfIO
{
  TEST_CASE("raw: a CLASS kept as its groups, no base taken from it")
  {
    const std::string_view text = "  0\r\nCLASS\r\n  1\r\nACDBPLACEHOLDER\r\n"
                                  "  2\r\nAcDbPlaceHolder\r\n 90\r\n        0\r\n";
    const auto             all  = Tags::All(text);
    REQUIRE(all);
    File::Cursor in{*all};
    const auto   record = in.Take();
    REQUIRE(record);
    const std::vector<Raw> taken = Copied(std::span{&*record, 1}, Fields::Reading{});
    REQUIRE(taken.size()     == 1);
    CHECK(taken.front().Type == "CLASS");
    CHECK(taken.front().Groups.size() == 3);
    Fields::Sheet page;
    REQUIRE(Put(page.To, taken));
    CHECK(page.Out == text);
  }
} // namespace DxfIO
