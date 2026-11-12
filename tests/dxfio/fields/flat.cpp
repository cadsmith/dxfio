#include "fields/flat.hpp"

#include "fields/read.hpp"
#include "fixture.hpp"
#include "tags/tags.hpp"

#include <doctest/doctest.h>

#include <tuple>

namespace DxfIO::Fields
{
  namespace
  {
    struct Vertex
    {
      Vector Location{};
    };

    const std::tuple Shapes{Flat{.Code = 10, .Member = &Vertex::Location}};
  } // namespace

  TEST_CASE("flat: a point's x and y, nothing else written")
  {
    const auto all = Tags::All(" 10\n1.0\n 20\n2.0\n");
    REQUIRE(all);
    Walk   in{*all, Reading{}};
    Vertex taken{};
    Read(taken, in, Shapes);
    CHECK(taken.Location == Vector{.X = 1, .Y = 2, .Z = 0});
    CHECK(in.Rest().empty());
    Sheet page;
    REQUIRE(Write(taken, page.To, Shapes));
    CHECK(page.Out == " 10\r\n1.0\r\n 20\r\n2.0\r\n");
  }

  TEST_CASE("flat: a z a file gave, kept after its y")
  {
    const auto all = Tags::All(" 10\n1.0\n 20\n2.0\n 30\n9.0\n");
    REQUIRE(all);
    Walk   in{*all, Reading{}};
    Vertex taken{};
    Read(taken, in, Shapes);
    CHECK(taken.Location == Vector{.X = 1, .Y = 2, .Z = 9});
    Sheet page;
    REQUIRE(Write(taken, page.To, Shapes));
    CHECK(page.Out == " 10\r\n1.0\r\n 20\r\n2.0\r\n 30\r\n9.0\r\n");
  }
} // namespace DxfIO::Fields
