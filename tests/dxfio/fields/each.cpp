#include "fields/each.hpp"

#include "fields/count.hpp"
#include "fields/field.hpp"
#include "fields/flat.hpp"
#include "fields/numbers.hpp"
#include "fields/subclass.hpp"
#include "fixture.hpp"
#include "tags/tags.hpp"

#include <doctest/doctest.h>

#include <tuple>
#include <vector>

namespace DxfIO::Fields
{
namespace
{
struct Vertex
{
  Vector Location{};
  double Bulge{};

  auto operator==(const Vertex &) const -> bool = default;
};

struct Polyline
{
  std::vector<Vertex> Vertices;
  int Flags{};
};

const std::tuple Corners{
    Flat{.Code = 10, .Member = &Vertex::Location},
    Field{.Code = 42, .Member = &Vertex::Bulge, .Fallback = 0.0},
};

const std::tuple Shapes{
    Count{.Code = 90, .Member = &Polyline::Vertices},
    Field{.Code = 70, .Member = &Polyline::Flags},
    Each{.Opener = 10, .Member = &Polyline::Vertices, .Fields = Corners},
};
} // namespace

TEST_CASE("each: an element's groups never slip to the next")
{
  const auto all = Tags::All(" 90\n3\n 70\n1\n 10\n0.0\n 20\n0.0\n 42\n0.5\n"
                             " 10\n1.0\n 20\n0.0\n 10\n1.0\n 20\n1.0\n"
                             " 42\n-1.0\n");
  REQUIRE(all);
  Walk in{*all, Reading{}};
  Polyline taken{};
  Read(taken, in, Shapes);
  CHECK(taken.Flags == 1);
  REQUIRE(taken.Vertices.size() == 3);
  CHECK(taken.Vertices.at(0).Bulge == 0.5);
  CHECK(taken.Vertices.at(1).Bulge == 0.0);
  CHECK(taken.Vertices.at(2).Bulge == -1.0);
  CHECK(taken.Vertices.at(2).Location == Vector{.X = 1, .Y = 1, .Z = 0});
  CHECK(in.Rest().empty());
}

TEST_CASE("each: a count past what the record holds, its elements alone")
{
  const auto all = Tags::All(" 90\n1000000000\n 10\n0.0\n 20\n0.0\n");
  REQUIRE(all);
  Walk in{*all, Reading{}};
  Polyline taken{};
  Read(taken, in, Shapes);
  CHECK(taken.Vertices.size() == 1);
}

TEST_CASE("each: the count written as the list's size, its elements after")
{
  Polyline shown{};
  shown.Vertices = {Vertex{.Location = {.X = 1, .Y = 2, .Z = 0}, .Bulge = 0.5}, Vertex{}};
  Sheet page;
  REQUIRE(Write(shown, page.To, Shapes));
  CHECK(page.Out == " 90\r\n        2\r\n 70\r\n     0\r\n"
                    " 10\r\n1.0\r\n 20\r\n2.0\r\n 42\r\n0.5\r\n"
                    " 10\r\n0.0\r\n 20\r\n0.0\r\n");
}

TEST_CASE("each: its elements found wherever the field before it stopped")
{
  const std::tuple fields{
      Subclass{.Marker = "AcDbLine"},
      Count{.Code = 90, .Member = &Polyline::Vertices},
      Each{.Opener = 10,
           .Member = &Polyline::Vertices,
           .Fields = std::tuple{Flat{.Code = 10, .Member = &Vertex::Location}}},
  };
  const auto all = Tags::All("100\nAcDbLine\n 10\n1.0\n 20\n2.0\n 90\n1\n");
  REQUIRE(all);
  Walk in{*all, Reading{}};
  Polyline taken{};
  Read(taken, in, fields);
  REQUIRE(taken.Vertices.size() == 1);
  CHECK(taken.Vertices.front().Location.X == 1);
}
} // namespace DxfIO::Fields
