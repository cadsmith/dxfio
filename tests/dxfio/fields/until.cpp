#include "fields/until.hpp"

#include "fields/field.hpp"
#include "fields/numbers.hpp"
#include "fixture.hpp"
#include "tags/tags.hpp"

#include <doctest/doctest.h>

#include <tuple>

namespace DxfIO::Fields
{
  namespace
  {
    struct Layer
    {
      int  Color{};
      bool Plot{true};
    };

    const std::tuple Shapes{
        Field{.Code = 62, .Member = &Layer::Color},
        Until{.Release = Version::R14,
              .Fields  = std::tuple{Field{.Code = 290, .Member = &Layer::Plot}}},
    };
  } // namespace

  TEST_CASE("until: fields read only up to their release")
  {
    const auto all = Tags::All(" 62\n1\n290\n0\n");
    REQUIRE(all);
    Walk  later{*all, Reading{}};
    Layer taken{};
    Read(taken, later, Shapes);
    CHECK(taken.Color == 1);
    CHECK(taken.Plot);
    Walk old{*all, Reading{.Release = Version::R12, .Text = Encoding::Utf8()}};
    Read(taken, old, Shapes);
    CHECK_FALSE(taken.Plot);
  }

  TEST_CASE("until: fields written only up to their release")
  {
    Sheet later;
    REQUIRE(Write(Layer{.Color = 1, .Plot = false}, later.To, Shapes));
    CHECK(later.Out == " 62\r\n     1\r\n");
    Sheet old{Version::R12};
    REQUIRE(Write(Layer{.Color = 1, .Plot = false}, old.To, Shapes));
    CHECK(old.Out == " 62\r\n     1\r\n290\r\n     0\r\n");
  }
} // namespace DxfIO::Fields
