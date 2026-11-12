#include "fields/subclass.hpp"

#include "fields/field.hpp"
#include "fields/numbers.hpp"
#include "fields/read.hpp"
#include "fixture.hpp"
#include "tags/tags.hpp"

#include <doctest/doctest.h>

#include <tuple>

namespace DxfIO::Fields
{
  namespace
  {
    struct Circle
    {
      double Base{};
      double Radius{};
    };

    const std::tuple Shapes{
        Subclass{.Marker = "AcDbEntity"},
        Field{.Code = 40, .Member = &Circle::Base},
        Subclass{.Marker = "AcDbCircle"},
        Field{.Code = 40, .Member = &Circle::Radius},
    };
  } // namespace

  TEST_CASE("subclass: the same code read in each subclass's fence")
  {
    const auto all = Tags::All("100\nAcDbEntity\n 40\n1.0\n100\nAcDbCircle\n"
                               " 40\n2.0\n");
    REQUIRE(all);
    Walk   in{*all, Reading{}};
    Circle taken{};
    Read(taken, in, Shapes);
    CHECK(taken.Base   == 1.0);
    CHECK(taken.Radius == 2.0);
  }

  TEST_CASE("subclass: a marker written from R13 only")
  {
    const Circle shown{.Base = 1, .Radius = 2};
    Sheet        modern;
    REQUIRE(Write(shown, modern.To, Shapes));
    CHECK(modern.Out
          == "100\r\nAcDbEntity\r\n 40\r\n1.0\r\n"
             "100\r\nAcDbCircle\r\n 40\r\n2.0\r\n");
    Sheet old{Version::R12};
    REQUIRE(Write(shown, old.To, Shapes));
    CHECK(old.Out == " 40\r\n1.0\r\n 40\r\n2.0\r\n");
  }
} // namespace DxfIO::Fields
