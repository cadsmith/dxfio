#include "tail.hpp"

#include "fields/fixture.hpp"
#include "tags/tags.hpp"

#include <doctest/doctest.h>

#include <format>
#include <string>
#include <vector>

namespace DxfIO
{
TEST_CASE("tail: extended data an application each, a repeat dropped")
{
  const auto all = Tags::All("  8\n0\n1001\nACAD\n1000\nx\n1001\nKADXY\n"
                             "1070\n3\n1001\nACAD\n1000\ny\n");
  REQUIRE(all);
  const std::size_t at = Trailing(*all);
  CHECK(at == 1);
  Base taken{};
  std::vector<Issue> issues;
  Tail(std::span{*all}.subspan(at), taken, Fields::Reading{.Issues = &issues});
  REQUIRE(taken.Extended.size() == 2);
  CHECK(taken.Extended.front().Application == "ACAD");
  CHECK(taken.Extended.front().Groups.size() == 1);
  REQUIRE(issues.size() == 1);
  CHECK(issues.front().What == "extended data of ACAD given again, the first kept");
}

TEST_CASE("tail: thousands of applications read each once, in their order, in their time")
{
  std::string text;
  for (int at = 0; at < 20000; ++at)
  {
    text += std::format("1001\nAPP{}\n1070\n1\n", at);
  }
  text += "1001\nAPP0\n";
  const auto all = Tags::All(text);
  REQUIRE(all);
  Base taken{};
  std::vector<Issue> issues;
  Tail(*all, taken, Fields::Reading{.Issues = &issues});
  REQUIRE(taken.Extended.size() == 20000);
  CHECK(taken.Extended.front().Application == "APP0");
  CHECK(taken.Extended.back().Application == "APP19999");
  CHECK(issues.size() == 1);
}

TEST_CASE("tail: written an application each")
{
  Base shown{};
  shown.Extended = {Xdata{.Application = "ACAD", .Groups = {Group{.Code = 1070, .Value = std::int64_t{3}}}}};
  Fields::Sheet page;
  REQUIRE(Tail(page.To, shown));
  CHECK(page.Out == "1001\r\nACAD\r\n1070\r\n     3\r\n");
}
} // namespace DxfIO

namespace DxfIO
{
TEST_CASE("tail: a group before its application left out and told")
{
  const auto all = Tags::All("1000\nx\n1001\nACAD\n1000\ny\n");
  REQUIRE(all);
  Base taken{};
  std::vector<Issue> told;
  Tail(std::span{*all}, taken, Fields::Reading{.Issues = &told});
  REQUIRE(taken.Extended.size() == 1);
  CHECK(taken.Extended.front().Groups.size() == 1);
  REQUIRE(told.size() == 1);
  CHECK(told.front().What == "group 1000 before its application, left out");
}
} // namespace DxfIO
