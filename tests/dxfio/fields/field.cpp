#include "fields/field.hpp"

#include "fields/numbers.hpp"
#include "fields/points.hpp"
#include "fields/read.hpp"
#include "fields/text.hpp"
#include "fixture.hpp"
#include "tags/tags.hpp"
#include "tracked.hpp"

#include <doctest/doctest.h>

#include <string>
#include <string_view>
#include <tuple>

namespace DxfIO::Fields
{
  namespace
  {
    struct Line
    {
      double Thickness{};
      Vector Start{};
      Vector Extrusion{Up};
      Symbol Layer{"0"};
      int    Flags{};
    };

    constexpr std::tuple Shapes{
        Field{.Code = 8, .Member = &Line::Layer, .Fallback = std::string_view{"0"}},
        Field{.Code = 39, .Member = &Line::Thickness, .Fallback = 0.0},
        Field{.Code = 10, .Member = &Line::Start},
        Field{.Code = 70, .Member = &Line::Flags, .Since = Version::R2000},
        Field{.Code = 210, .Member = &Line::Extrusion, .Fallback = Up},
    };
  } // namespace

  TEST_CASE("field: a member from its group, its fallback when none")
  {
    const auto all = Tags::All(" 10\n1.0\n 20\n2.0\n 39\n0.5\n");
    REQUIRE(all);
    Walk in{*all, Reading{}};
    Line taken{.Thickness = 9, .Extrusion = {}};
    Read(taken, in, Shapes);
    CHECK(taken.Layer     == std::string_view{"0"});
    CHECK(taken.Thickness == 0.5);
    CHECK(taken.Start     == Vector{.X = 1, .Y = 2, .Z = 0});
    CHECK(taken.Extrusion == Up);
  }

  TEST_CASE("field: a member at its fallback is left out on write")
  {
    Sheet page;
    Line  plain{};
    plain.Start = Vector{.X = 1, .Y = 2, .Z = 0};
    plain.Flags = 1;
    REQUIRE(Write(plain, page.To, Shapes));
    CHECK(page.Out
          == " 10\r\n1.0\r\n 20\r\n2.0\r\n 30\r\n0.0\r\n"
             " 70\r\n     1\r\n");
  }

  TEST_CASE("field: a release older than the field's neither reads nor writes it")
  {
    const auto all = Tags::All(" 70\n3\n");
    REQUIRE(all);
    Walk in{*all, Reading{.Release = Version::R12, .Text = Encoding::Utf8()}};
    Line taken{};
    Read(taken, in, Shapes);
    CHECK(taken.Flags == 0);
    Sheet page{Version::R12};
    Line  flagged{};
    flagged.Flags = 3;
    REQUIRE(Write(flagged, page.To, Shapes));
    CHECK(page.Out.find(" 70") == std::string::npos);
  }

  TEST_CASE("field: the value read is moved into the member, not copied")
  {
    struct Holder
    {
      Tracked Held;
    };

    const auto all = Tags::All("  1\nx\n");
    REQUIRE(all);
    Walk   in{*all, Reading{}};
    Holder taken{};
    Take(Field{.Code = 1, .Member = &Holder::Held}, taken, in);
    CHECK_FALSE(taken.Held.Copied);
  }
} // namespace DxfIO::Fields
