#include "fields/numbers.hpp"

#include "fixture.hpp"
#include "tags/tags.hpp"
#include "unknown/io.hpp"

#include <doctest/doctest.h>

#include <cstdint>
#include <vector>

namespace DxfIO::Fields
{
  TEST_CASE("numbers: integers, reals and flags as their groups hold them")
  {
    const auto all = Tags::All(" 70\n     5\n 90\n70000\n160\n42\n 40\n2.5\n"
                               "290\n     1\n 66\n     1\n");
    REQUIRE(all);
    Walk in{*all, Reading{}};
    CHECK(Value<int>::Read(in, 70)           == 5);
    CHECK(Value<int>::Read(in, 90)           == 70000);
    CHECK(Value<std::int64_t>::Read(in, 160) == 42);
    CHECK(Value<double>::Read(in, 40)        == 2.5);
    CHECK(Value<bool>::Read(in, 290)         == true);
    CHECK(Value<bool>::Read(in, 66)          == true);
    CHECK_FALSE(Value<double>::Read(in, 41));
  }

  TEST_CASE("numbers: an integer too large for an int left untaken, told")
  {
    const auto all = Tags::All("160\n4294967297\n");
    REQUIRE(all);
    std::vector<Issue> issues;
    Walk               in{*all, Reading{.Issues = &issues}};
    CHECK_FALSE(Value<int>::Read(in, 160));
    CHECK(in.Kept().empty());
    REQUIRE(issues.size()     == 1);
    CHECK(issues.front().What == "group 160 holds no value its field takes");
  }

  TEST_CASE("numbers: written in the columns of their code")
  {
    Sheet page;
    REQUIRE(Value<int>::Write(page.To, 70, 5));
    REQUIRE(Value<double>::Write(page.To, 40, 2.5));
    REQUIRE(Value<bool>::Write(page.To, 290, true));
    REQUIRE(Value<std::int64_t>::Write(page.To, 90, 70000));
    CHECK(page.Out
          == " 70\r\n     5\r\n 40\r\n2.5\r\n290\r\n     1\r\n"
             " 90\r\n    70000\r\n");
  }

  TEST_CASE("numbers: a text where an integer is wanted is no integer")
  {
    const auto all = Tags::All("  1\nwalls\n");
    REQUIRE(all);
    Walk in{*all, Reading{}};
    CHECK_FALSE(Value<int>::Read(in, 1));
  }
} // namespace DxfIO::Fields
