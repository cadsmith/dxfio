#include "fields/handles.hpp"

#include "fixture.hpp"
#include "tags/tags.hpp"
#include "unknown/io.hpp"

#include <doctest/doctest.h>

#include <string>
#include <vector>

namespace DxfIO::Fields
{
TEST_CASE("handles: read from their hex, none when it is no hex")
{
  const auto all = Tags::All("330\n1F\n  5\nARROW\n");
  REQUIRE(all);
  Walk in{*all, Reading{}};
  CHECK(Value<Handle>::Read(in, 330) == Handle{0x1f});
  CHECK_FALSE(Value<Handle>::Read(in, 5));
}

TEST_CASE("handles: one that is no hex left untaken, told as such")
{
  const auto all = Tags::All("340\nXYZ\n340\n1F\n");
  REQUIRE(all);
  std::vector<Issue> issues;
  Walk in{*all, Reading{.Issues = &issues}};
  CHECK(Value<Handle>::Read(in, 340) == Handle{0x1f});
  CHECK_FALSE(Value<Handle>::Read(in, 340));
  CHECK(in.Kept().empty());
  REQUIRE(issues.size() == 1);
  CHECK(issues.front().What == "group 340 holds no value its field takes");
}

TEST_CASE("handles: one that is no hex past another field's marker kept")
{
  const auto all = Tags::All("100\nAcDbOne\n340\nXYZ\n100\nAcDbTwo\n");
  REQUIRE(all);
  std::vector<Issue> issues;
  Walk in{*all, Reading{.Release = Version::R2000, .Issues = &issues}};
  REQUIRE(in.Subclass("AcDbOne"));
  CHECK_FALSE(Value<Handle>::Read(in, 340));
  REQUIRE(in.Subclass("AcDbTwo"));
  const std::vector<Group> kept = in.Kept();
  CHECK(issues.empty());
  REQUIRE(kept.size() == 1);
  CHECK(kept.front() == Group{.Code = 340, .Value = std::string{"XYZ"}});
}

TEST_CASE("handles: written in upper case hex")
{
  Sheet page;
  REQUIRE(Value<Handle>::Write(page.To, 330, Handle{0x2c20a}));
  CHECK(page.Out == "330\r\n2C20A\r\n");
}
} // namespace DxfIO::Fields
