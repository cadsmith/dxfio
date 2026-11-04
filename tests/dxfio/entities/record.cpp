#include "entities/shapes/record.hpp"

#include "../fields/fixture.hpp"
#include "fields/read.hpp"
#include "tags/tags.hpp"

#include <doctest/doctest.h>

namespace DxfIO::Entities
{
TEST_CASE("record: an entity's layer, and paper space only when set")
{
  const auto all = Tags::All("100\nAcDbEntity\n 67\n1\n  8\nwalls\n");
  REQUIRE(all);
  Fields::Walk in{*all, Fields::Reading{}};
  Record taken{};
  Fields::Read(taken, in, Shapes<Record>);
  CHECK(taken.Paper);
  CHECK(taken.Layer == "walls");
  Fields::Sheet page;
  REQUIRE(Fields::Write(Record{}, page.To, Shapes<Record>));
  CHECK(page.Out == "100\r\nAcDbEntity\r\n  8\r\n0\r\n");
}
} // namespace DxfIO::Entities
