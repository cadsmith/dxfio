#include "dxfio/unknown.hpp"

#include "fields/fixture.hpp"
#include "file/cursor.hpp"
#include "tags/tags.hpp"
#include "unknown/io.hpp"

#include <doctest/doctest.h>

#include <string_view>
#include <vector>

namespace DxfIO
{
  TEST_CASE("unknown: a record kept whole and written back as read")
  {
    const std::string_view text = "  0\r\nLINE\r\n  5\r\n2A\r\n330\r\n1F\r\n100\r\nAcDbEntity\r\n"
                                  "  8\r\n0\r\n100\r\nAcDbLine\r\n 10\r\n1.0\r\n1001\r\nACAD\r\n"
                                  "1000\r\nx\r\n";
    const auto             all  = Tags::All(text);
    REQUIRE(all);
    File::Cursor in{*all};
    const auto   record = in.Take();
    REQUIRE(record);
    std::vector<Issue> issues;
    const Unknown      taken = Kept(*record, Fields::Reading{.Issues = &issues});
    CHECK(taken.Type          == "LINE");
    CHECK(taken.Base.Self     == Handle{0x2a});
    CHECK(taken.Base.Owner    == Handle{0x1f});
    CHECK(taken.Groups.size() == 4);
    CHECK(taken.Base.Extended.size() == 1);
    Fields::Sheet page;
    REQUIRE(Put(page.To, taken));
    CHECK(page.Out == text);
  }
} // namespace DxfIO
