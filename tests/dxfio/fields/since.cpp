#include "fields/since.hpp"

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
        Since{.Release = Version::R2000,
              .Fields  = std::tuple{Field{.Code = 290, .Member = &Layer::Plot}}},
    };
  } // namespace

  TEST_CASE("since: fields read only from their release")
  {
    const auto all = Tags::All(" 62\n1\n290\n0\n");
    REQUIRE(all);
    Walk  old{*all, Reading{.Release = Version::R12, .Text = Encoding::Utf8()}};
    Layer taken{};
    Read(taken, old, Shapes);
    CHECK(taken.Color == 1);
    CHECK(taken.Plot);
    Walk later{*all, Reading{}};
    Read(taken, later, Shapes);
    CHECK_FALSE(taken.Plot);
  }

  TEST_CASE("since: fields written only from their release")
  {
    Sheet old{Version::R12};
    REQUIRE(Write(Layer{.Color = 1, .Plot = false}, old.To, Shapes));
    CHECK(old.Out == " 62\r\n     1\r\n");
    Sheet later;
    REQUIRE(Write(Layer{.Color = 1, .Plot = false}, later.To, Shapes));
    CHECK(later.Out == " 62\r\n     1\r\n290\r\n     0\r\n");
  }
} // namespace DxfIO::Fields
