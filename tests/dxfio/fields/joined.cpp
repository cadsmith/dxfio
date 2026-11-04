#include "fields/joined.hpp"

#include "fields/field.hpp"
#include "fields/numbers.hpp"
#include "fields/read.hpp"
#include "fields/subclass.hpp"
#include "fixture.hpp"
#include "tags/tags.hpp"
#include "unknown/io.hpp"

#include <doctest/doctest.h>

#include <string_view>
#include <tuple>
#include <vector>

namespace DxfIO::Fields
{
namespace
{
struct Word
{
  double Height{};
  int Vertical{};
};

const std::tuple Shapes{
    Subclass{.Marker = "AcDbEntity"},
    Joined{.Marker = "AcDbText"},
    Field{.Code = 40, .Member = &Word::Height},
    Joined{.Marker = "AcDbText"},
    Field{.Code = 73, .Member = &Word::Vertical, .Fallback = 0},
};

struct Outcome
{
  Word Taken{};
  std::vector<Group> Kept;
};

auto Read(std::string_view text, Version release = Version::R2018) -> Outcome
{
  const auto all = Tags::All(text);
  REQUIRE(all);
  std::vector<Issue> issues;
  Walk in{*all, Reading{.Release = release, .Issues = &issues}};
  Outcome out{};
  Fields::Read(out.Taken, in, Shapes);
  out.Kept = in.Kept();
  CHECK(issues.empty());
  return out;
}
} // namespace

TEST_CASE("joined: both markers one fence, each where it was written")
{
  const Outcome one = Read("100\nAcDbEntity\n100\nAcDbText\n 40\n2.0\n"
                           "100\nAcDbText\n 73\n     1\n");
  CHECK(one.Taken.Height == 2);
  CHECK(one.Taken.Vertical == 1);
  CHECK(one.Kept.empty());
}

TEST_CASE("joined: a field on either side of a marker found")
{
  CHECK(Read("100\nAcDbEntity\n100\nAcDbText\n 73\n     1\n100\nAcDbText\n"
             " 40\n2.0\n")
            .Taken.Height == 2);
  CHECK(Read("100\nAcDbEntity\n 40\n2.0\n100\nAcDbText\n 73\n     1\n").Taken.Height == 2);
  CHECK(Read("100\nAcDbEntity\n100\nAcDbText\n 40\n2.0\n 73\n     1\n").Taken.Vertical == 1);
}

TEST_CASE("joined: the last group of a code wins, the other told")
{
  const auto all = Tags::All("100\nAcDbEntity\n100\nAcDbText\n 40\n1.0\n"
                             "100\nAcDbText\n 40\n2.0\n");
  REQUIRE(all);
  std::vector<Issue> issues;
  Walk in{*all, Reading{.Issues = &issues}};
  Word taken{};
  Fields::Read(taken, in, Shapes);
  CHECK(taken.Height == 2);
  CHECK(in.Kept().empty());
  CHECK(issues.size() == 1);
}

TEST_CASE("joined: a marker past another class's reached, asked again a no-op")
{
  const auto all = Tags::All("100\nAcDbEntity\n100\nAcDbText\n 40\n2.0\n"
                             "100\nAcDbText\n 73\n     1\n100\nAcDbThing\n"
                             "100\nAcDbText\n");
  REQUIRE(all);
  std::vector<Issue> issues;
  Walk in{*all, Reading{.Issues = &issues}};
  Word taken{};
  Fields::Read(taken, in, Shapes);
  CHECK(taken.Vertical == 1);
  CHECK(in.Kept().size() == 1);
  CHECK(issues.empty());
}

TEST_CASE("joined: any number of markers taken, another name ends the fence")
{
  CHECK(Read("100\nAcDbEntity\n100\nAcDbText\n100\nAcDbText\n100\nAcDbText\n"
             " 73\n     1\n")
            .Kept.empty());
  const Outcome other = Read("100\nAcDbEntity\n100\nAcDbText\n 40\n2.0\n"
                             "100\nAcDbThing\n 40\n9.0\n");
  CHECK(other.Taken.Height == 2);
  CHECK(other.Kept.size() == 2);
}

TEST_CASE("joined: an embedded object ends the fence")
{
  const Outcome inner = Read("100\nAcDbEntity\n100\nAcDbText\n 40\n2.0\n"
                             "101\nEmbedded Object\n 40\n9.0\n");
  CHECK(inner.Taken.Height == 2);
}

TEST_CASE("joined: before R13 the record is whole, a marker no marker")
{
  const Outcome old = Read("100\nAcDbText\n 40\n2.0\n 73\n     1\n", Version::R12);
  CHECK(old.Taken.Height == 2);
  CHECK(old.Taken.Vertical == 1);
  CHECK(old.Kept.size() == 1);
}

TEST_CASE("joined: with no such marker the record is whole, in its order")
{
  const Outcome none = Read("100\nAcDbEntity\n100\nAcDbThing\n 40\n2.0\n"
                            " 73\n     1\n");
  CHECK(none.Taken.Height == 2);
  CHECK(none.Taken.Vertical == 1);
  CHECK(none.Kept.size() == 1);
}

TEST_CASE("joined: found before AcDbEntity, the fence its own")
{
  const Outcome early = Read("100\nAcDbText\n 40\n2.0\n100\nAcDbEntity\n"
                             "100\nAcDbText\n 73\n     1\n");
  CHECK(early.Taken.Height == 2);
  CHECK(early.Taken.Vertical == 1);
  CHECK(early.Kept.empty());
}

TEST_CASE("joined: a second run past another class's marker, kept apart")
{
  const Outcome split = Read("100\nAcDbEntity\n100\nAcDbText\n 40\n2.0\n"
                             "100\nAcDbThing\n 90\n1\n"
                             "100\nAcDbText\n 73\n     1\n");
  CHECK(split.Taken.Height == 2);
  CHECK(split.Taken.Vertical == 1);
  CHECK(split.Kept.size() == 2);
}

TEST_CASE("joined: another class's groups before it are not taken")
{
  const Outcome after = Read("100\nAcDbEntity\n100\nAcDbThing\n 40\n9.0\n"
                             "100\nAcDbText\n 73\n     1\n");
  CHECK(after.Taken.Height == 0);
  CHECK(after.Taken.Vertical == 1);
  CHECK(after.Kept.size() == 2);
}

TEST_CASE("joined: one that cannot take its group told, whichever fence")
{
  const auto all = Tags::All("100\nAcDbEntity\n100\nAcDbText\n"
                             "160\n99999999999\n100\nAcDbText\n");
  REQUIRE(all);
  const std::tuple fitted{
      Subclass{.Marker = "AcDbEntity"},
      Joined{.Marker = "AcDbText"},
      Field{.Code = 160, .Member = &Word::Vertical, .Fallback = 0},
      Joined{.Marker = "AcDbText"},
  };
  std::vector<Issue> issues;
  Walk in{*all, Reading{.Issues = &issues}};
  Word taken{};
  Fields::Read(taken, in, fitted);
  CHECK(in.Kept().empty());
  CHECK(issues.size() == 1);
}

TEST_CASE("joined: written from R13")
{
  Sheet page{Version::R2018};
  REQUIRE(Mark(Joined{.Marker = "AcDbText"}, page.To));
  CHECK(page.Out == "100\r\nAcDbText\r\n");
  Sheet old{Version::R12};
  REQUIRE(Mark(Joined{.Marker = "AcDbText"}, old.To));
  CHECK(old.Out.empty());
}
} // namespace DxfIO::Fields
