#include "fields/part.hpp"

#include "fields/field.hpp"
#include "fields/numbers.hpp"
#include "fields/text.hpp"
#include "fixture.hpp"
#include "tags/tags.hpp"

#include <doctest/doctest.h>

#include <tuple>

namespace DxfIO::Fields
{
  namespace
  {
    struct Common
    {
      Symbol Layer{};
    };

    struct Point
    {
      Common Base{};
      double Thickness{};
    };

    struct Boxy
    {
      Boxed<Common> Base{};
    };

    const std::tuple Commons{Field{.Code = 8, .Member = &Common::Layer}};

    const std::tuple Shapes{
        Part{.Member = &Point::Base, .Fields = Commons},
        Field{.Code = 39, .Member = &Point::Thickness},
    };

    const std::tuple Boxes{Part{.Member = &Boxy::Base, .Fields = Commons}};
  } // namespace

  TEST_CASE("part: a member in a box read and written through it")
  {
    const auto all = Tags::All("  8\nwalls\n");
    REQUIRE(all);
    Walk in{*all, Reading{}};
    Boxy taken{};
    Read(taken, in, Boxes);
    CHECK(taken.Base->Layer == Symbol{"walls"});
    Sheet page;
    REQUIRE(Write(taken, page.To, Boxes));
    CHECK(page.Out == "  8\r\nwalls\r\n");
  }

  TEST_CASE("part: a member read and written by its own fields")
  {
    const auto all = Tags::All("  8\nwalls\n 39\n2.0\n");
    REQUIRE(all);
    Walk  in{*all, Reading{}};
    Point taken{};
    Read(taken, in, Shapes);
    CHECK(taken.Base.Layer == Symbol{"walls"});
    CHECK(taken.Thickness  == 2.0);
    Sheet page;
    REQUIRE(Write(taken, page.To, Shapes));
    CHECK(page.Out == "  8\r\nwalls\r\n 39\r\n2.0\r\n");
  }
} // namespace DxfIO::Fields
