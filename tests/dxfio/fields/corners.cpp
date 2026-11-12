#include "fields/corners.hpp"

#include "fixture.hpp"
#include "tags/tags.hpp"

#include <doctest/doctest.h>

#include <string>

namespace DxfIO::Fields
{
  namespace
  {
    constexpr Vector First{.X = 1, .Y = 2, .Z = 3};
  } // namespace

  TEST_CASE("corners: four points on one code after another")
  {
    const auto all = Tags::All(" 10\n1.0\n 20\n2.0\n 30\n3.0\n"
                               " 11\n4.0\n 12\n5.0\n 13\n6.0\n");
    REQUIRE(all);
    Walk       in{*all, Reading{}};
    const auto taken = Value<Corners>::Read(in, 10);
    REQUIRE(taken);
    CHECK(taken->front() == First);
    CHECK(taken->at(1).X == 4);
    CHECK(taken->at(3).X == 6);
  }

  TEST_CASE("corners: a fourth not given the same as the third")
  {
    const auto all = Tags::All(" 10\n1.0\n 11\n2.0\n 12\n3.0\n 22\n4.0\n");
    REQUIRE(all);
    Walk       in{*all, Reading{}};
    const auto taken = Value<Corners>::Read(in, 10);
    REQUIRE(taken);
    CHECK(taken->at(3) == Vector{.X = 3, .Y = 4, .Z = 0});
  }

  TEST_CASE("corners: none when the record gives none")
  {
    const auto all = Tags::All(" 40\n1.0\n");
    REQUIRE(all);
    Walk in{*all, Reading{}};
    CHECK_FALSE(Value<Corners>::Read(in, 10));
  }

  TEST_CASE("corners: written as four points, each on its own code")
  {
    Sheet         page;
    const Corners held{First, Vector{}, Vector{}, Vector{.X = 9, .Y = 0, .Z = 0}};
    REQUIRE(Value<Corners>::Write(page.To, 10, held));
    CHECK(page.Out
          == " 10\r\n1.0\r\n 20\r\n2.0\r\n 30\r\n3.0\r\n"
             " 11\r\n0.0\r\n 21\r\n0.0\r\n 31\r\n0.0\r\n"
             " 12\r\n0.0\r\n 22\r\n0.0\r\n 32\r\n0.0\r\n"
             " 13\r\n9.0\r\n 23\r\n0.0\r\n 33\r\n0.0\r\n");
  }
} // namespace DxfIO::Fields
