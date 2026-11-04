#include "fields/widths.hpp"

#include "fixture.hpp"
#include "tags/tags.hpp"
#include "unknown/io.hpp"

#include <doctest/doctest.h>

#include <array>
#include <string_view>
#include <utility>
#include <vector>

namespace DxfIO::Fields
{
TEST_CASE("widths: both, or an end alone, its start 0")
{
  constexpr std::array<std::pair<std::string_view, Widths>, 2> given{{
      {"40\n1.5\n41\n2.5\n", {.Start = 1.5, .End = 2.5}},
      {"41\n2.5\n", {.Start = 0, .End = 2.5}},
  }};
  for (const auto &[text, wanted] : given)
  {
    CAPTURE(text);
    const auto all = Tags::All(text);
    REQUIRE(all);
    Walk in{*all, Reading{}};
    CHECK(Value<Widths>::Read(in, 40) == wanted);
  }
}

TEST_CASE("widths: none when the record gives neither")
{
  const auto all = Tags::All("42\n1.5\n");
  REQUIRE(all);
  Walk in{*all, Reading{}};
  CHECK_FALSE(Value<Widths>::Read(in, 40));
}

TEST_CASE("widths: a start alone is none, told and not written back")
{
  const auto all = Tags::All("40\n1.5\n");
  REQUIRE(all);
  std::vector<Issue> issues;
  Walk in{*all, Reading{.Issues = &issues}};
  CHECK_FALSE(Value<Widths>::Read(in, 40));
  CHECK(in.Kept().empty());
  CHECK(issues.size() == 1);
}

TEST_CASE("widths: written both, 0 too")
{
  Sheet to;
  REQUIRE(Value<Widths>::Write(to.To, 40, Widths{.Start = 1.5}));
  CHECK(to.Out == " 40\r\n1.5\r\n 41\r\n0.0\r\n");
}
} // namespace DxfIO::Fields
