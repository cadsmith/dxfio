#include "fields/every.hpp"

#include "fields/field.hpp"
#include "fields/handles.hpp"
#include "fields/numbers.hpp"
#include "fields/text.hpp"
#include "fixture.hpp"
#include "tags/tags.hpp"
#include "unknown/io.hpp"

#include <doctest/doctest.h>

#include <string>
#include <tuple>
#include <vector>

namespace DxfIO::Fields
{
namespace
{
struct Entry
{
  std::string Name;
  Handle Value{};

  auto operator==(const Entry &) const -> bool = default;
};

struct Dictionary
{
  int Cloning{};
  std::vector<Entry> Entries;
};

const std::tuple Named{
    Field{.Code = 3, .Member = &Entry::Name},
    Field{.Code = 350, .Member = &Entry::Value, .Fallback = Handle{}},
};

const std::tuple Shapes{
    Field{.Code = 281, .Member = &Dictionary::Cloning},
    Every{.Opener = 3, .Member = &Dictionary::Entries, .Fields = Named},
};
} // namespace

TEST_CASE("every: an element for each opener, its groups kept to it")
{
  const auto all = Tags::All("281\n1\n  3\nA\n350\n1A\n  3\nB\n"
                             "  3\nC\n350\n2C\n");
  REQUIRE(all);
  Walk in{*all, Reading{}};
  Dictionary taken{};
  Read(taken, in, Shapes);
  CHECK(taken.Cloning == 1);
  CHECK(taken.Entries == std::vector<Entry>{
                             Entry{.Name = "A", .Value = Handle{.Value = 0x1a}},
                             Entry{.Name = "B", .Value = Handle{}},
                             Entry{.Name = "C", .Value = Handle{.Value = 0x2c}},
                         });
}

TEST_CASE("every: an opener no field takes still moves the list on")
{
  const std::tuple values{Field{.Code = 350, .Member = &Entry::Value, .Fallback = Handle{}}};
  const auto all = Tags::All("  3\nA\n350\n1\n  3\nB\n350\n2\n");
  REQUIRE(all);
  Walk in{*all, Reading{}};
  Dictionary taken{};
  Read(taken, in, std::tuple{Every{.Opener = 3, .Member = &Dictionary::Entries, .Fields = values}});
  REQUIRE(taken.Entries.size() == 2);
  CHECK(taken.Entries.at(1).Value == Handle{.Value = 2});
}

TEST_CASE("every: an opener before the fields read so far still taken")
{
  const auto all = Tags::All("  3\nA\n350\n1A\n281\n1\n");
  REQUIRE(all);
  Walk in{*all, Reading{}};
  Dictionary taken{};
  Read(taken, in, Shapes);
  CHECK(taken.Cloning == 1);
  REQUIRE(taken.Entries.size() == 1);
  CHECK(taken.Entries.front().Name == "A");
}

TEST_CASE("every: written an element after another")
{
  Sheet page;
  Dictionary one{};
  one.Entries = {Entry{.Name = "A", .Value = Handle{.Value = 0x1a}}};
  REQUIRE(Write(one, page.To, Shapes));
  CHECK(page.Out == "281\r\n     0\r\n  3\r\nA\r\n350\r\n1A\r\n");
}

TEST_CASE("every: a list a count sized holds what its openers made")
{
  const auto all = Tags::All("  3\nfirst\n350\n1F\n");
  REQUIRE(all);
  Walk in{*all, Reading{}};
  Dictionary taken{};
  taken.Entries.assign(5, Entry{});
  Read(taken, in, Shapes);
  REQUIRE(taken.Entries.size() == 1);
  CHECK(taken.Entries.front().Name == "first");
}

TEST_CASE("every: an opener before the marker is no element written back")
{
  const auto all = Tags::All("100\nAcDbObject\n  3\nstray\n100\nAcDbDictionary\n");
  REQUIRE(all);
  std::vector<Issue> issues;
  Walk in{*all, Reading{.Issues = &issues}};
  REQUIRE(in.Subclass("AcDbObject"));
  REQUIRE(in.Subclass("AcDbDictionary"));
  Dictionary taken{};
  Read(taken, in, Shapes);
  CHECK(taken.Entries.empty());
  CHECK(in.Kept().empty());
  CHECK(issues.size() == 1);
}
} // namespace DxfIO::Fields
