#include "fields/embedded.hpp"

#include "fields/field.hpp"
#include "fields/numbers.hpp"
#include "fields/subclass.hpp"
#include "fixture.hpp"
#include "tags/tags.hpp"
#include "unknown/io.hpp"

#include <doctest/doctest.h>

#include <optional>
#include <string_view>
#include <tuple>
#include <vector>

namespace DxfIO::Fields
{
namespace
{
struct Inner
{
  double Width{};
};

struct Outer
{
  double Width{};
  std::optional<Inner> Nested;
};

const std::tuple Nested{Field{.Code = 40, .Member = &Inner::Width}};

const std::tuple Shapes{
    Subclass{.Marker = "AcDbThing"},
    Field{.Code = 40, .Member = &Outer::Width},
    Embedded{.Member = &Outer::Nested, .Fields = Nested},
};

struct Outcome
{
  Outer Taken{};
  std::vector<Group> Kept;
};

auto Read(std::string_view text) -> Outcome
{
  const auto all = Tags::All(text);
  REQUIRE(all);
  std::vector<Issue> issues;
  Walk in{*all, Reading{.Issues = &issues}};
  Outcome out{};
  Fields::Read(out.Taken, in, Shapes);
  out.Kept = in.Kept();
  return out;
}
} // namespace

TEST_CASE("embedded: its groups its own, whatever the code")
{
  const Outcome one = Read("100\nAcDbThing\n 40\n1.0\n"
                           "101\nEmbedded Object\n 40\n2.0\n");
  CHECK(one.Taken.Width == 1);
  REQUIRE(one.Taken.Nested);
  CHECK(one.Taken.Nested->Width == 2);
  CHECK(one.Kept.empty());
}

TEST_CASE("embedded: none in the record, none held")
{
  CHECK_FALSE(Read("100\nAcDbThing\n 40\n1.0\n").Taken.Nested);
}

TEST_CASE("embedded: any value opens it, as AutoCAD reads it")
{
  CHECK(Read("100\nAcDbThing\n101\nElse\n 40\n2.0\n").Taken.Nested);
}

TEST_CASE("embedded: found wherever the record holds it")
{
  CHECK(Read(" 40\n1.0\n101\nEmbedded Object\n 40\n2.0\n").Taken.Nested);
  const Outcome one = Read("101\nEmbedded Object\n100\nAcDbThing\n 40\n1.0\n");
  CHECK(one.Taken.Nested);
  CHECK(one.Taken.Width == 1);
  CHECK(one.Kept.empty());
}

TEST_CASE("embedded: a repeat past it dropped, a group nobody asks for kept")
{
  const Outcome one = Read("100\nAcDbThing\n 40\n1.0\n 39\n1.0\n"
                           "101\nEmbedded Object\n 40\n2.0\n 40\n3.0\n");
  REQUIRE(one.Taken.Nested);
  CHECK(one.Taken.Nested->Width == 3);
  REQUIRE(one.Kept.size() == 1);
  CHECK(one.Kept.front().Code == 39);
}

TEST_CASE("embedded: written after what comes before it")
{
  const Outer one{.Width = 1, .Nested = Inner{.Width = 2}};
  Sheet page;
  REQUIRE(Write(one, page.To, Shapes));
  CHECK(page.Out == "100\r\nAcDbThing\r\n 40\r\n1.0\r\n"
                    "101\r\nEmbedded Object\r\n 40\r\n2.0\r\n");
}
} // namespace DxfIO::Fields
