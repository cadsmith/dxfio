#include "fields/either.hpp"

#include "fields/field.hpp"
#include "fields/numbers.hpp"
#include "fields/read.hpp"
#include "fixture.hpp"
#include "tags/tags.hpp"
#include "unknown/io.hpp"

#include <doctest/doctest.h>

#include <cstddef>
#include <tuple>
#include <vector>

namespace DxfIO::Fields
{
  namespace
  {
    struct Kind
    {
      int Flags{};
    };

    auto Chosen(const Kind &one) -> std::size_t
    {
      return one.Flags == 0 ? 0 : 1;
    }

    const std::tuple Shapes{
        Either<Kind, 2>{.Markers = {"AcDbFlat", "AcDbSolid"}, .Pick = Chosen},
        Field{.Code = 70, .Member = &Kind::Flags, .Fallback = 0},
    };
  } // namespace

  TEST_CASE("either: the fence of whichever marker the record holds")
  {
    const auto all = Tags::All("100\nAcDbSolid\n 70\n8\n");
    REQUIRE(all);
    std::vector<Issue> issues;
    Walk               in{*all, Reading{.Issues = &issues}};
    Kind               taken{};
    Read(taken, in, Shapes);
    CHECK(taken.Flags == 8);
    CHECK(in.Kept().empty());
    CHECK(issues.empty());
  }

  TEST_CASE("either: whichever comes first in the record, not in the list")
  {
    const auto all = Tags::All("100\nAcDbSolid\n 70\n8\n100\nAcDbFlat\n");
    REQUIRE(all);
    Walk in{*all, Reading{}};
    Kind taken{};
    Read(taken, in, Shapes);
    CHECK(taken.Flags == 8);
  }

  TEST_CASE("either: written as what the record holds picks")
  {
    Sheet flat;
    REQUIRE(Write(Kind{}, flat.To, Shapes));
    CHECK(flat.Out == "100\r\nAcDbFlat\r\n");
    Sheet solid;
    REQUIRE(Write(Kind{.Flags = 8}, solid.To, Shapes));
    CHECK(solid.Out == "100\r\nAcDbSolid\r\n 70\r\n     8\r\n");
  }

  TEST_CASE("either: before R13 no marker, and none needed")
  {
    const auto all = Tags::All(" 70\n8\n");
    REQUIRE(all);
    Walk in{*all, Reading{.Release = Version::R12}};
    Kind taken{};
    Read(taken, in, Shapes);
    CHECK(taken.Flags == 8);
    Sheet page{Version::R12};
    REQUIRE(Write(taken, page.To, Shapes));
    CHECK(page.Out == " 70\r\n     8\r\n");
  }
} // namespace DxfIO::Fields
