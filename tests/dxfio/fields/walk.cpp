#include "fields/walk.hpp"

#include "fields/elements.hpp"
#include "fields/scratch.hpp"
#include "tags/tags.hpp"
#include "unknown/io.hpp"

#include <doctest/doctest.h>

#include <array>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace DxfIO::Fields
{
namespace
{
auto Real(const Tags::Tag *one) -> double
{
  REQUIRE(one != nullptr);
  return std::get<double>(one->Value);
}

auto Large(const Tags::Tag &one) -> bool
{
  return std::get<double>(one.Value) > 5.0;
}
} // namespace

TEST_CASE("walk: a scratch lends its room to one walk at a time, the next finds its own")
{
  const auto all = Tags::All("  8\nwalls\n 40\n1.0\n 40\n9.0\n");
  REQUIRE(all);
  Scratch scratch;
  const Reading with{.Reuse = &scratch};
  {
    Walk first{*all, with};
    CHECK(scratch.Busy);
    CHECK(Real(first.Take(40)) == 1.0);
    Walk second{*all, with};
    CHECK(Real(second.Take(40)) == 1.0);
    CHECK(Real(second.Take(40)) == 9.0);
    CHECK(Real(first.Take(40)) == 9.0);
  }
  CHECK_FALSE(scratch.Busy);
  CHECK(scratch.Used.capacity() >= all->size());
  Walk again{*all, with};
  CHECK(scratch.Busy);
  CHECK(again.Take(8) != nullptr);
  CHECK(again.Rest().size() == 2);
}

TEST_CASE("lease: assigned to itself, through another name, a lease keeps its hold")
{
  Scratch scratch;
  Lease held{&scratch};
  Lease &same = held;
  held = same;
  CHECK(held.From() == &scratch);
  held = std::move(same);
  CHECK(held.From() == &scratch);
  Lease none;
  Lease &alias = none;
  none = std::move(alias);
  CHECK(none.From() == nullptr);
}

TEST_CASE("walk: a copy holds no scratch, a move carries the hold, an assigned walk keeps its own")
{
  const auto all = Tags::All("  8\nwalls\n 40\n1.0\n");
  REQUIRE(all);
  Scratch scratch;
  const Reading with{.Reuse = &scratch};
  {
    Walk held{*all, with};
    {
      const Walk copy = held;
      CHECK(scratch.Busy);
    }
    CHECK(scratch.Busy);
    Walk other{*all, Reading{}};
    other = held;
    other = std::move(held);
    CHECK(scratch.Busy);
    held = std::move(other);
    CHECK(scratch.Busy);
    Walk moved{std::move(held)};
    Walk plain{*all, Reading{}};
    moved = std::move(plain);
    CHECK(scratch.Busy);
    CHECK(Real(moved.Take(40)) == 1.0);
  }
  CHECK_FALSE(scratch.Busy);
}

TEST_CASE("walk: a field takes the next group of its code, wrapping round")
{
  const auto all = Tags::All(" 40\n1.0\n 41\n2.0\n 40\n3.0\n");
  REQUIRE(all);
  Walk in{*all, Reading{}};
  CHECK(Real(in.Take(41)) == 2.0);
  CHECK(Real(in.Take(40)) == 3.0);
  CHECK(Real(in.Take(40)) == 1.0);
  CHECK(in.Take(40) == nullptr);
}

TEST_CASE("walk: a subclass fences its fields to the next marker")
{
  const auto all = Tags::All("100\nAcDbEntity\n 40\n1.0\n100\nAcDbCircle\n"
                             " 40\n2.0\n");
  REQUIRE(all);
  Walk in{*all, Reading{}};
  REQUIRE(in.Subclass("AcDbCircle"));
  CHECK(Real(in.Take(40)) == 2.0);
  CHECK(in.Take(40) == nullptr);
  REQUIRE(in.Subclass("AcDbEntity"));
  CHECK(Real(in.Take(40)) == 1.0);
}

TEST_CASE("walk: without its marker or before R13 the record is whole")
{
  const auto all = Tags::All("100\nAcDbCircle\n 40\n2.0\n");
  REQUIRE(all);
  Walk missing{*all, Reading{}};
  CHECK_FALSE(missing.Subclass("AcDbLine"));
  CHECK(Real(missing.Take(40)) == 2.0);
  Walk old{*all, Reading{.Release = Version::R12}};
  CHECK_FALSE(old.Subclass("AcDbCircle"));
  CHECK(old.Take(100) != nullptr);
}

TEST_CASE("walk: an element of a list is fenced to the next of its code")
{
  const auto all = Tags::All(" 10\n1.0\n 42\n0.5\n 10\n2.0\n 10\n3.0\n"
                             " 42\n0.7\n");
  REQUIRE(all);
  Walk in{*all, Reading{}};
  const Walk::Fence outer = in.Fenced();
  std::vector<double> bulges;
  for (int element = 0; element < 3; ++element)
  {
    in.Narrow(10, outer);
    CHECK(in.Take(10) != nullptr);
    const Tags::Tag *bulge = in.Take(42);
    bulges.push_back(bulge == nullptr ? 0.0 : Real(bulge));
  }
  in.Restore(outer);
  CHECK(bulges == std::vector<double>{0.5, 0.0, 0.7});
}

TEST_CASE("walk: of several markers the first in the record")
{
  const auto all = Tags::All("100\nAcDbB\n  1\nb\n100\nAcDbA\n  1\na\n");
  REQUIRE(all);
  Walk in{*all, Reading{}};
  const std::array<std::string_view, 2> either{"AcDbA", "AcDbB"};
  REQUIRE(in.Subclass(std::span{either}));
  CHECK(Tags::Text(*in.Take(1)) == "b");
}

TEST_CASE("walk: inside a marker a field takes the last of its code")
{
  const auto all = Tags::All("100\nAcDbLine\n  1\none\n  8\na\n  1\ntwo\n");
  REQUIRE(all);
  std::vector<Issue> issues;
  Walk in{*all, Reading{.Issues = &issues}};
  REQUIRE(in.Subclass("AcDbLine"));
  const Tags::Tag *taken = in.Take(1);
  REQUIRE(taken != nullptr);
  CHECK(Tags::Text(*taken) == "two");
  CHECK(in.Take(8) != nullptr);
  CHECK(in.Kept().empty());
  REQUIRE(issues.size() == 1);
  CHECK(issues.front().Line == 3);
  CHECK(issues.front().What == "group 1 given again, one kept");
}

TEST_CASE("walk: with no marker a code's groups go to its fields in order")
{
  const auto all = Tags::All("  1\none\n  1\ntwo\n");
  REQUIRE(all);
  Walk in{*all, Reading{}};
  CHECK_FALSE(in.Subclass("AcDbPlotSettings"));
  CHECK(Tags::Text(*in.Take(1)) == "one");
  CHECK_FALSE(in.Subclass("AcDbLayout"));
  CHECK(Tags::Text(*in.Take(1)) == "two");
}

TEST_CASE("walk: a marker missing after one found goes back to their order")
{
  const auto all = Tags::All("100\nAcDbEntity\n  1\none\n  1\ntwo\n");
  REQUIRE(all);
  Walk in{*all, Reading{}};
  REQUIRE(in.Subclass("AcDbEntity"));
  CHECK_FALSE(in.Subclass("AcDbLine"));
  CHECK(Tags::Text(*in.Take(1)) == "one");
}

TEST_CASE("walk: a record with no marker keeps what no field took")
{
  const auto all = Tags::All("  8\na\n 39\n1.0\n");
  REQUIRE(all);
  std::vector<Issue> issues;
  Walk in{*all, Reading{.Issues = &issues}};
  CHECK_FALSE(in.Subclass("AcDbLine"));
  CHECK(in.Take(8) != nullptr);
  CHECK(in.Kept().size() == 1);
  CHECK(issues.empty());
}

TEST_CASE("walk: before R13 a 100 is a group, what stands before it kept")
{
  const auto all = Tags::All("  8\na\n100\nx\n");
  REQUIRE(all);
  std::vector<Issue> issues;
  Walk in{*all, Reading{.Release = Version::R12, .Issues = &issues}};
  CHECK(in.Kept().size() == 2);
  CHECK(issues.empty());
}

TEST_CASE("walk: past an earlier marker a code of the last fields dropped")
{
  const auto all = Tags::All("100\nAcDbOther\n 70\n3\n100\nAcDbSymbolTable\n 70\n0\n");
  REQUIRE(all);
  std::vector<Issue> issues;
  Walk in{*all, Reading{.Issues = &issues}};
  REQUIRE(in.Subclass("AcDbOther"));
  REQUIRE(in.Subclass("AcDbSymbolTable"));
  CHECK(in.Take(70) != nullptr);
  CHECK(in.Kept().empty());
  CHECK(issues.size() == 1);
}

TEST_CASE("walk: a group before the first marker never written back")
{
  const auto all = Tags::All("330\n2\n  8\na\n100\nAcDbLine\n");
  REQUIRE(all);
  std::vector<Issue> issues;
  Walk in{*all, Reading{.Issues = &issues}};
  REQUIRE(in.Subclass("AcDbLine"));
  CHECK(in.Kept().empty());
  CHECK(issues.size() == 2);
}

TEST_CASE("walk: a brace past a marker kept, as a BLOCK_RECORD's BLKREFS")
{
  const auto all = Tags::All("100\nAcDbLine\n102\n{BLKREFS\n331\n1F\n102\n}\n");
  REQUIRE(all);
  std::vector<Issue> issues;
  Walk in{*all, Reading{.Issues = &issues}};
  REQUIRE(in.Subclass("AcDbLine"));
  CHECK(in.Kept().size() == 3);
  CHECK(issues.empty());
}

TEST_CASE("walk: a list's elements in their order, each its own fence")
{
  const auto all = Tags::All("  3\nA\n  1\nx\n  3\nB\n  1\ny\n");
  REQUIRE(all);
  Walk in{*all, Reading{}};
  std::string seen;
  Elements(in, 3,
           [&] -> void
           {
             seen += Tags::Text(*in.Take(3));
             seen += Tags::Text(*in.Take(1));
           });
  CHECK(seen == "AxBy");
}

TEST_CASE("walk: the rest leaves out a repeat of a code the part's fields took, told")
{
  const auto all = Tags::All("  8\nwalls\n 40\n1.0\n 40\n9.0\n  3\nnote\n");
  REQUIRE(all);
  std::vector<Issue> issues;
  Walk in{*all, Reading{.Issues = &issues}};
  CHECK(in.Take(8) != nullptr);
  CHECK(in.Take(40) != nullptr);
  const std::vector<Group> rest = in.Rest();
  REQUIRE(rest.size() == 1);
  CHECK(rest.front().Code == 3);
  REQUIRE(issues.size() == 1);
  CHECK(issues.front().Line == 5);
  CHECK(issues.front().What == "group 40 given again, one kept");
}

TEST_CASE("walk: the rest leaves out a repeat before the part of a code its fields took, told")
{
  const auto all = Tags::All("  8\nwalls\n  2\nearly\n100\nAcDbBlockReference\n  2\nname\n  3\nnote\n");
  REQUIRE(all);
  std::vector<Issue> issues;
  Walk in{*all, Reading{.Issues = &issues}};
  CHECK(in.Take(8) != nullptr);
  REQUIRE(in.Subclass("AcDbBlockReference"));
  const Tags::Tag *name = in.Take(2);
  REQUIRE(name != nullptr);
  CHECK(std::get<std::string_view>(name->Value) == "name");
  const std::vector<Group> rest = in.Rest();
  REQUIRE(rest.size() == 1);
  CHECK(rest.front().Code == 3);
  REQUIRE(issues.size() == 1);
  CHECK(issues.front().Line == 3);
  CHECK(issues.front().What == "group 2 given again, one kept");
}

TEST_CASE("walk: the rest keeps a repeat before the part under a marker no field took")
{
  const auto all = Tags::All("  8\nwalls\n100\nAcDbTable\n  2\nearly\n100\nAcDbBlockReference\n  2\nname\n  3\nnote\n");
  REQUIRE(all);
  std::vector<Issue> issues;
  Walk in{*all, Reading{.Issues = &issues}};
  CHECK(in.Take(8) != nullptr);
  REQUIRE(in.Subclass("AcDbBlockReference"));
  CHECK(in.Take(2) != nullptr);
  const std::vector<Group> rest = in.Rest();
  REQUIRE(rest.size() == 3);
  CHECK(rest.at(0).Code == 100);
  CHECK(rest.at(1).Code == 2);
  CHECK(rest.at(2).Code == 3);
  CHECK(issues.empty());
}

TEST_CASE("walk: the rest keeps a repeat before the part under an embedded object's marker")
{
  const auto all = Tags::All("  8\nwalls\n101\nEmbedded Object\n  2\nearly\n100\nAcDbBlockReference\n  2\nname\n");
  REQUIRE(all);
  std::vector<Issue> issues;
  Walk in{*all, Reading{.Issues = &issues}};
  CHECK(in.Take(8) != nullptr);
  REQUIRE(in.Subclass("AcDbBlockReference"));
  CHECK(in.Take(2) != nullptr);
  const std::vector<Group> rest = in.Rest();
  REQUIRE(rest.size() == 2);
  CHECK(rest.at(0).Code == 101);
  CHECK(rest.at(1).Code == 2);
  CHECK(issues.empty());
}

TEST_CASE("walk: before R13 the rest keeps a repeat of a code a field took")
{
  const auto all = Tags::All("  8\nwalls\n 40\n1.0\n 40\n9.0\n  3\nnote\n");
  REQUIRE(all);
  std::vector<Issue> issues;
  Walk in{*all, Reading{.Release = Version::R12, .Issues = &issues}};
  CHECK(in.Take(8) != nullptr);
  CHECK(Real(in.Take(40)) == 1.0);
  const std::vector<Group> rest = in.Rest();
  REQUIRE(rest.size() == 2);
  CHECK(rest.front().Code == 40);
  CHECK(issues.empty());
}

TEST_CASE("walk: the rest keeps a repeat of a code an earlier part took")
{
  const auto all = Tags::All("  8\nwalls\n100\nAcDbLine\n 40\n1.0\n  8\nagain\n  3\nnote\n");
  REQUIRE(all);
  std::vector<Issue> issues;
  Walk in{*all, Reading{.Issues = &issues}};
  CHECK(in.Take(8) != nullptr);
  REQUIRE(in.Subclass("AcDbLine"));
  CHECK(in.Take(40) != nullptr);
  const std::vector<Group> rest = in.Rest();
  REQUIRE(rest.size() == 2);
  CHECK(rest.front().Code == 8);
  CHECK(rest.back().Code == 3);
  CHECK(issues.empty());
}

TEST_CASE("walk: the rest keeps a repeat its field found unfit")
{
  const auto all = Tags::All("  8\nwalls\n 40\n1.0\n 40\n9.0\n");
  REQUIRE(all);
  std::vector<Issue> issues;
  Walk in{*all, Reading{.Issues = &issues}};
  CHECK(in.Take(8) != nullptr);
  CHECK(Real(in.Take(40, Large)) == 9.0);
  const std::vector<Group> rest = in.Rest();
  REQUIRE(rest.size() == 1);
  CHECK(rest.front().Code == 40);
  CHECK(issues.empty());
}

TEST_CASE("walk: read in order, the rest keeps a repeat of a code a field took")
{
  const auto all = Tags::All("100\nAcDbVisualStyle\n  2\nnamed\n 70\n4\n 70\n58\n 90\n1\n");
  REQUIRE(all);
  std::vector<Issue> issues;
  Walk in{*all, Reading{.Issues = &issues}};
  in.Sequence("AcDbVisualStyle");
  CHECK(in.Take(2) != nullptr);
  CHECK(in.Take(70) != nullptr);
  const std::vector<Group> rest = in.Rest();
  REQUIRE(rest.size() == 2);
  CHECK(rest.front().Code == 70);
  CHECK(rest.back().Code == 90);
  CHECK(issues.empty());
}

TEST_CASE("walk: what no field took is handed back, a repeat told")
{
  const auto all = Tags::All("  8\nwalls\n 40\n1.0\n 40\n9.0\n999\nnote\n");
  REQUIRE(all);
  std::vector<Issue> issues;
  Walk in{*all, Reading{.Issues = &issues}};
  CHECK(in.Take(8) != nullptr);
  CHECK(in.Take(40) != nullptr);
  const std::vector<Group> kept = in.Kept();
  REQUIRE(kept.empty());
  REQUIRE(issues.size() == 1);
  CHECK(issues.front().Line == 5);
  CHECK(issues.front().What == "group 40 given again, one kept");
}

TEST_CASE("walk: a group before its subclass the last fields read dropped, the record not whole")
{
  const auto all = Tags::All(" 70\n3\n100\nAcDbSymbolTable\n 70\n0\n");
  REQUIRE(all);
  std::vector<Issue> issues;
  Walk in{*all, Reading{.Issues = &issues}};
  CHECK(in.Subclass("AcDbSymbolTable"));
  CHECK(in.Take(70) != nullptr);
  CHECK(in.Whole());
  CHECK(in.Kept().empty());
  CHECK_FALSE(in.Whole());
  REQUIRE(issues.size() == 1);
  CHECK(issues.front().Line == 1);
  CHECK(issues.front().What == "group 70 out of its place, dropped");
}

TEST_CASE("walk: before R13 a group any field read is dropped, not the last")
{
  const auto all = Tags::All("350\n3\n  3\nE\n");
  REQUIRE(all);
  std::vector<Issue> issues;
  Walk in{*all, Reading{.Release = Version::R12, .Issues = &issues}};
  CHECK_FALSE(in.Subclass("AcDbDictionary"));
  const Walk::Fence outer = in.Fenced();
  in.Narrow(3, outer);
  CHECK(in.Take(3) != nullptr);
  CHECK(in.Take(350) == nullptr);
  in.Restore(outer);
  CHECK_FALSE(in.Subclass("AcDbDictionaryWithDefault"));
  CHECK(in.Take(340) == nullptr);
  CHECK(in.Kept().empty());
  CHECK_FALSE(in.Whole());
  REQUIRE(issues.size() == 1);
  CHECK(issues.front().What == "group 350 out of its place, dropped");
}

TEST_CASE("walk: a group after a marker no field took is kept")
{
  const auto all = Tags::All("100\nAcDbLine\n 39\n1.0\n100\nAcDbNext\n 39\n2.0\n");
  REQUIRE(all);
  std::vector<Issue> issues;
  Walk in{*all, Reading{.Issues = &issues}};
  CHECK(in.Subclass("AcDbLine"));
  CHECK(in.Take(39) != nullptr);
  CHECK(in.Kept().size() == 2);
  CHECK(in.Whole());
  CHECK(issues.empty());
}

TEST_CASE("walk: counts claim from what the record holds")
{
  const auto all = Tags::All(" 90\n1000000\n 10\n1.0\n");
  REQUIRE(all);
  Walk in{*all, Reading{}};
  CHECK(in.Claim(1000000) == 2);
}

TEST_CASE("walk: of two codes, the one given later")
{
  const auto all = Tags::All("100\nM\n 11\n1.0\n 50\n2.0\n");
  REQUIRE(all);
  Walk in{*all, Reading{}};
  REQUIRE(in.Subclass("M"));
  CHECK(in.Later(50, 11));
  CHECK_FALSE(in.Later(11, 50));
  CHECK(in.Later(11, 7));
  CHECK_FALSE(in.Later(7, 11));
  CHECK_FALSE(in.Later(7, 8));
}

TEST_CASE("walk: with no marker, later goes by what a field would take")
{
  const auto all = Tags::All(" 11\n1.0\n 50\n2.0\n 11\n3.0\n");
  REQUIRE(all);
  Walk in{*all, Reading{}};
  CHECK(in.Later(50, 11));
}

TEST_CASE("walk: gathered in the order of the file, each taken once")
{
  constexpr std::array codes{1, 3};
  const auto all = Tags::All("  1\na\n  3\nb\n 40\n1.0\n  1\nc\n");
  REQUIRE(all);
  Walk in{*all, Reading{}};
  std::string joined;
  for (const Tags::Tag *one : in.Gather(codes))
  {
    joined += Tags::Text(*one);
  }
  CHECK(joined == "abc");
  CHECK(in.Gather(codes).empty());
  CHECK(in.Rest().size() == 1);
}

TEST_CASE("walk: a record no field can hold says so")
{
  const auto all = Tags::All("100\nAcDbHatch\n");
  REQUIRE(all);
  Walk in{*all, Reading{}};
  in.Sequence("AcDbHatch");
  CHECK(in.Whole());
  in.Refuse();
  CHECK_FALSE(in.Whole());
}
} // namespace DxfIO::Fields
