#include "file/cursor.hpp"

#include "tags/tags.hpp"

#include <doctest/doctest.h>

#include <span>
#include <string_view>

namespace DxfIO::File
{
TEST_CASE("cursor: a record runs from its group 0 to the next")
{
  const auto all = Tags::All("  0\nLINE\n  8\n0\n 10\n1.0\n  0\nCIRCLE\n"
                             " 40\n2.0\n");
  REQUIRE(all);
  Cursor in{*all};
  const auto line = in.Take();
  REQUIRE(line);
  CHECK(line->Name == "LINE");
  CHECK(line->Tags.size() == 3);
  const auto circle = in.Take("CIRCLE");
  REQUIRE(circle);
  CHECK(circle->Tags.size() == 2);
  CHECK(in.Done());
  CHECK(in.Take() == std::unexpected{Failure{9, "group 0 expected, found none"}});
}

TEST_CASE("cursor: a record not opened by a group 0 is a failure")
{
  const auto all = Tags::All("  8\n0\n");
  REQUIRE(all);
  Cursor in{*all};
  CHECK(in.Take() == std::unexpected{Failure{1, "group 0 expected, found group 8"}});
}

TEST_CASE("cursor: a record of another name is a failure")
{
  const auto all = Tags::All("  0\nBLOCK\n");
  REQUIRE(all);
  Cursor in{*all};
  CHECK(in.Take("TABLE") == std::unexpected{Failure{1, "TABLE expected, found BLOCK"}});
}

TEST_CASE("cursor: the records up to one named, which is left")
{
  const auto all = Tags::All("  0\nLAYER\n  2\n0\n  0\nLAYER\n  2\nwalls\n  0\nENDTAB\n");
  REQUIRE(all);
  Cursor in{*all};
  const auto layers = in.Until("ENDTAB");
  REQUIRE(layers);
  CHECK(layers->size() == 2);
  const auto end = in.Take("ENDTAB");
  REQUIRE(end);
  CHECK(in.Done());
}

TEST_CASE("cursor: the records up to one never found are a failure")
{
  const auto all = Tags::All("  0\nLAYER\n  2\n0\n");
  REQUIRE(all);
  Cursor in{*all};
  CHECK(in.Until("ENDTAB") == std::unexpected{Failure{3, "ENDTAB missing"}});
}

TEST_CASE("cursor: an empty run of tags has no line")
{
  Cursor in{std::span<const Tags::Tag>{}};
  CHECK(in.Done());
  const auto none = in.Take();
  REQUIRE_FALSE(none);
  CHECK(none.error().Line == 0);
}

TEST_CASE("cursor: a group where a record should open stops a run")
{
  const auto all = Tags::All("  8\n0\n  0\nENDSEC\n");
  REQUIRE(all);
  Cursor in{*all};
  const auto run = in.Until("ENDSEC");
  REQUIRE_FALSE(run);
  CHECK(run.error().What == "group 0 expected, found group 8");
}
} // namespace DxfIO::File

namespace DxfIO::File
{
TEST_CASE("cursor: a record asked by name where none is left is a failure")
{
  Cursor in{std::span<const Tags::Tag>{}};
  CHECK_FALSE(in.Take("SECTION"));
}
} // namespace DxfIO::File
