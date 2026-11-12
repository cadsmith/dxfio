#include "fields/many.hpp"

#include "fields/handles.hpp"
#include "fields/read.hpp"
#include "fixture.hpp"
#include "tags/tags.hpp"
#include "tracked.hpp"

#include <doctest/doctest.h>

#include <tuple>
#include <vector>

namespace DxfIO::Fields
{
  namespace
  {
    struct Dictionary
    {
      std::vector<Handle> Entries;
    };

    const std::tuple Shapes{Many{.Code = 350, .Member = &Dictionary::Entries}};
  } // namespace

  TEST_CASE("many: every group of a code, in their order")
  {
    const auto all = Tags::All("350\n1A\n  3\nx\n350\n1B\n");
    REQUIRE(all);
    Walk       in{*all, Reading{}};
    Dictionary taken{};
    Read(taken, in, Shapes);
    CHECK(taken.Entries == std::vector<Handle>{Handle{0x1a}, Handle{0x1b}});
    Sheet page;
    REQUIRE(Write(taken, page.To, Shapes));
    CHECK(page.Out == "350\r\n1A\r\n350\r\n1B\r\n");
  }

  TEST_CASE("many: a group that is not the element's passed over")
  {
    const auto all = Tags::All("350\nXYZ\n350\n1B\n");
    REQUIRE(all);
    Walk       in{*all, Reading{}};
    Dictionary taken{};
    Read(taken, in, Shapes);
    CHECK(taken.Entries == std::vector<Handle>{Handle{0x1b}});
  }

  TEST_CASE("many: each element read is moved in, not copied")
  {
    struct Holder
    {
      std::vector<Tracked> Held;
    };

    const auto all = Tags::All("  1\nx\n  1\ny\n");
    REQUIRE(all);
    Walk   in{*all, Reading{}};
    Holder taken{};
    Take(Many{.Code = 1, .Member = &Holder::Held}, taken, in);
    REQUIRE(taken.Held.size() == 2);
    CHECK_FALSE(taken.Held.front().Copied);
    CHECK_FALSE(taken.Held.back().Copied);
  }
} // namespace DxfIO::Fields
