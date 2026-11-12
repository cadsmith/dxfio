#include "head.hpp"

#include "fields/fixture.hpp"
#include "tags/tags.hpp"

#include <doctest/doctest.h>

#include <span>
#include <string>
#include <vector>

namespace DxfIO
{
  namespace
  {
    auto Headed(std::span<const Tags::Tag> body, Base &into, const Fields::Reading &with = {},
                int code = Handled) -> std::vector<Tags::Tag>
    {
      std::vector<Issue> issues;
      Fields::Reading    bound = with;
      bound.Issues             = &issues;
      auto rest                = Head(body, Heading{.Into = &into, .Code = code}, bound);
      CHECK(issues.empty());
      return rest;
    }
  } // namespace

  TEST_CASE("head: a record's handle, braces and owner before its fields")
  {
    const auto all = Tags::All("  5\n2A\n102\n{ACAD_XDICTIONARY\n360\n2B\n"
                               "102\n}\n102\n{ACAD_REACTORS\n330\n1F\n102\n}\n"
                               "102\n{BLKREFS\n331\n3C\n102\n}\n330\n1\n"
                               "100\nAcDbEntity\n330\n7\n");
    REQUIRE(all);
    Base                         taken{};
    const std::vector<Tags::Tag> rest = Headed(*all, taken);
    CHECK(taken.Self            == Handle{0x2a});
    CHECK(taken.Dictionary      == Handle{0x2b});
    CHECK(taken.Reactors        == std::vector<Handle>{Handle{0x1f}});
    REQUIRE(taken.Braces.size() == 1);
    CHECK(taken.Braces.front().Name == "BLKREFS");
    CHECK(taken.Owner == Handle{1});
    CHECK(rest.size() == 2);
  }

  TEST_CASE("head: before R13 a 330 and a 102 are fields, no owner or brace")
  {
    const auto all = Tags::All("  5\n2A\n330\nC\n102\n{APP\n102\n}\n");
    REQUIRE(all);
    Base                         taken{};
    const std::vector<Tags::Tag> rest =
        Headed(*all, taken, Fields::Reading{.Release = Version::R12});
    CHECK(taken.Self == Handle{0x2a});
    CHECK_FALSE(taken.Owner);
    CHECK(taken.Braces.empty());
    CHECK(rest.size() == 3);
  }

  TEST_CASE("head: the handle on the code given, a DIMSTYLE's on 105")
  {
    const auto all = Tags::All("105\n1B0\n330\nA\n");
    REQUIRE(all);
    Base taken{};
    CHECK(Headed(*all, taken, Fields::Reading{}, 105).empty());
    CHECK(taken.Self  == Handle{0x1b0});
    CHECK(taken.Owner == Handle{0xa});
  }

  TEST_CASE("head: a 5 that is no hex stays among the fields")
  {
    const auto all = Tags::All("  5\nARROW\n");
    REQUIRE(all);
    Base taken{};
    CHECK(Headed(*all, taken).size() == 1);
    CHECK_FALSE(taken.Self);
  }

  TEST_CASE("head: the last handle given is the record's, a null one none")
  {
    const auto all = Tags::All("  5\n1F\n  5\n0\n  5\n2A\n");
    REQUIRE(all);
    Base               taken{};
    std::vector<Issue> issues;
    const auto rest = Head(*all, Heading{.Into = &taken}, Fields::Reading{.Issues = &issues});
    CHECK(taken.Self    == Handle{0x2a});
    REQUIRE(rest.size() == 1);
    CHECK(Tags::Text(rest.front()) == "0");
    REQUIRE(issues.size()          == 1);
    CHECK(issues.front().What      == "group 5 given again, one kept");
  }

  TEST_CASE("head: an open brace ends before the marker, the owner left it")
  {
    const auto all = Tags::All("  5\n2A\n102\n{ACAD_REACTORS\n330\n1F\n"
                               "330\n3C\n100\nAcDbEntity\n  8\n0\n");
    REQUIRE(all);
    Base               taken{};
    std::vector<Issue> issues;
    const auto rest = Head(*all, Heading{.Into = &taken}, Fields::Reading{.Issues = &issues});
    CHECK(taken.Reactors      == std::vector<Handle>{Handle{0x1f}});
    CHECK(taken.Owner         == Handle{0x3c});
    CHECK(rest.size()         == 2);
    CHECK(rest.front().Code   == 100);
    REQUIRE(issues.size()     == 1);
    CHECK(issues.front().Line == 3);
    CHECK(issues.front().What == "brace ACAD_REACTORS not closed");
  }

  TEST_CASE("head: an open brace before another keeps its handle a reactor")
  {
    const auto all = Tags::All("102\n{ACAD_REACTORS\n330\n1F\n"
                               "102\n{ACAD_XDICTIONARY\n360\n2A\n102\n}\n"
                               "330\n3C\n");
    REQUIRE(all);
    Base               taken{};
    std::vector<Issue> issues;
    static_cast<void>(Head(*all, Heading{.Into = &taken}, Fields::Reading{.Issues = &issues}));
    CHECK(taken.Reactors   == std::vector<Handle>{Handle{0x1f}});
    CHECK(taken.Dictionary == Handle{0x2a});
    CHECK(taken.Owner      == Handle{0x3c});
  }

  TEST_CASE("head: an app's open brace kept, closed when it is written")
  {
    const auto all = Tags::All("  5\n2A\n102\n{APP\n  1\nx\n100\nAcDbEntity\n");
    REQUIRE(all);
    Base               taken{};
    std::vector<Issue> issues;
    const auto rest = Head(*all, Heading{.Into = &taken}, Fields::Reading{.Issues = &issues});
    REQUIRE(taken.Braces.size() == 1);
    CHECK(taken.Braces.front().Groups.size() == 1);
    CHECK(rest.size()   == 1);
    CHECK(issues.size() == 1);
  }

  TEST_CASE("head: the last owner given, as AutoCAD keeps an object's")
  {
    const auto all = Tags::All("330\n1F\n330\n3C\n100\nAcDbEntity\n");
    REQUIRE(all);
    Base               taken{};
    std::vector<Issue> issues;
    const auto rest = Head(*all, Heading{.Into = &taken}, Fields::Reading{.Issues = &issues});
    CHECK(rest.size()         == 1);
    CHECK(taken.Owner         == Handle{0x3c});
    REQUIRE(issues.size()     == 1);
    CHECK(issues.front().What == "group 330 given again, one kept");
  }

  TEST_CASE("head: a second extension dictionary told as the group it replaced")
  {
    const auto all = Tags::All("102\n{ACAD_XDICTIONARY\n360\n1F\n102\n}\n"
                               "102\n{ACAD_XDICTIONARY\n360\n2A\n102\n}\n");
    REQUIRE(all);
    Base               taken{};
    std::vector<Issue> issues;
    static_cast<void>(Head(*all, Heading{.Into = &taken}, Fields::Reading{.Issues = &issues}));
    CHECK(taken.Dictionary == Handle{0x2a});
    CHECK(issues.size()    == 1);
  }

  TEST_CASE("head: braces past 64 deep stop nesting, so a head stays linear")
  {
    std::string text;
    for (int at = 0; at < 200; ++at)
    {
      text += "102\n{A\n";
    }
    const auto all = Tags::All(text);
    REQUIRE(all);
    Base               taken{};
    std::vector<Issue> issues;
    static_cast<void>(Head(*all, Heading{.Into = &taken}, Fields::Reading{.Issues = &issues}));
    CHECK(taken.Braces.size() == 200);
    CHECK(issues.size()       == 200);
  }

  TEST_CASE("head: what reactors hold no place for told, the rest kept")
  {
    const auto all = Tags::All("102\n{ACAD_REACTORS\n  1\nx\n330\n1F\n102\n}\n");
    REQUIRE(all);
    Base               taken{};
    std::vector<Issue> issues;
    static_cast<void>(Head(*all, Heading{.Into = &taken}, Fields::Reading{.Issues = &issues}));
    CHECK(taken.Reactors      == std::vector<Handle>{Handle{0x1f}});
    REQUIRE(issues.size()     == 1);
    CHECK(issues.front().What == "brace ACAD_REACTORS lost 1 of its groups");
  }

  TEST_CASE("head: a brace in a brace stays in it")
  {
    const auto all = Tags::All("102\n{A\n  1\nx\n102\n{B\n  1\ny\n102\n}\n"
                               "102\n}\n330\n1F\n");
    REQUIRE(all);
    Base       taken{};
    const auto rest = Headed(*all, taken);
    REQUIRE(taken.Braces.size()     == 1);
    CHECK(taken.Braces.front().Name == "A");
    CHECK(taken.Braces.front().Groups.size() == 4);
    CHECK(taken.Owner == Handle{0x1f});
    CHECK(rest.empty());
  }

  TEST_CASE("head: a brace with no end stops at the next one")
  {
    const auto all = Tags::All("102\n{A\n  1\nx\n102\n{B\n  1\ny\n102\n}\n"
                               "330\n1F\n");
    REQUIRE(all);
    Base               taken{};
    std::vector<Issue> issues;
    const auto rest = Head(*all, Heading{.Into = &taken}, Fields::Reading{.Issues = &issues});
    REQUIRE(taken.Braces.size()            == 2);
    CHECK(taken.Braces.at(0).Name          == "A");
    CHECK(taken.Braces.at(0).Groups.size() == 1);
    CHECK(taken.Braces.at(1).Name          == "B");
    CHECK(taken.Owner == Handle{0x1f});
    CHECK(rest.empty());
    CHECK(issues.size() == 1);
  }

  TEST_CASE("head: an extension dictionary's last handle, as AutoCAD holds")
  {
    const auto all = Tags::All("102\n{ACAD_XDICTIONARY\n360\n1F\n360\n2A\n  1\nx\n102\n}\n");
    REQUIRE(all);
    Base               taken{};
    std::vector<Issue> issues;
    static_cast<void>(Head(*all, Heading{.Into = &taken}, Fields::Reading{.Issues = &issues}));
    CHECK(taken.Dictionary == Handle{0x2a});
    CHECK(taken.Braces.empty());
    CHECK(issues.size() == 1);
  }

  TEST_CASE("head: written as AutoCAD writes it, braces and owner from R13")
  {
    Base shown{};
    shown.Self       = Handle{0x2a};
    shown.Owner      = Handle{1};
    shown.Dictionary = Handle{0x2b};
    shown.Reactors   = {Handle{0x1f}};
    Fields::Sheet modern;
    REQUIRE(Head(modern.To, shown, Handled));
    CHECK(modern.Out
          == "  5\r\n2A\r\n102\r\n{ACAD_XDICTIONARY\r\n360\r\n2B\r\n"
             "102\r\n}\r\n102\r\n{ACAD_REACTORS\r\n330\r\n1F\r\n"
             "102\r\n}\r\n330\r\n1\r\n");
    Fields::Sheet old{Version::R12};
    REQUIRE(Head(old.To, shown, Handled));
    CHECK(old.Out == "  5\r\n2A\r\n");
  }
} // namespace DxfIO

namespace DxfIO
{
  TEST_CASE("head: an extension dictionary naming none kept as a brace")
  {
    const auto all = Tags::All("  5\n2A\n102\n{ACAD_XDICTIONARY\n102\n}\n100\nAcDbEntity\n");
    REQUIRE(all);
    Base taken{};
    static_cast<void>(Headed(*all, taken));
    CHECK_FALSE(taken.Dictionary);
    REQUIRE(taken.Braces.size()     == 1);
    CHECK(taken.Braces.front().Name == "ACAD_XDICTIONARY");
  }

  TEST_CASE("head: a brace left open just before a marker, an owner not a handle")
  {
    const auto all = Tags::All("330\nZZ\n102\n{ACAD_REACTORS\n100\nAcDbEntity\n");
    REQUIRE(all);
    Base               taken{};
    std::vector<Issue> told;
    const auto         rest =
        Head(*all, Heading{.Into = &taken, .Code = Handled}, Fields::Reading{.Issues = &told});
    CHECK(taken.Reactors.empty());
    CHECK_FALSE(taken.Owner);
    CHECK_FALSE(rest.empty());
  }
} // namespace DxfIO
