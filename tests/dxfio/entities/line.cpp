#include "entities/shapes/line.hpp"

#include "../kind.hpp"
#include "drawn.hpp"
#include "file/cursor.hpp"
#include "filing.hpp"
#include "tags/tags.hpp"

#include <array>
#include <string_view>
#include <vector>

namespace DxfIO::Entities
{
TEST_CASE("line: a plain one, what it does not set left out")
{
  const Line taken = Replayed<Line>(Drawn("2CE"));
  CHECK(taken.End == Vector{.X = 100, .Y = 50, .Z = 0});
  CHECK(taken.Record.Color == Colors::Bylayer);
  CHECK(taken.Extrusion == Up);
}

TEST_CASE("line: every property of the entity part AutoCAD showed")
{
  const Line taken = Replayed<Line>(Drawn("2DB"));
  const Record &part = taken.Record;
  CHECK(part.Linetype == std::string_view{"DASHED"});
  CHECK(part.Material);
  CHECK(part.Color == 30);
  CHECK(part.Rgb == 16744448);
  CHECK(part.Transparency == 33554559);
  CHECK(part.Scale == 3);
  CHECK(part.Hidden);
  CHECK(part.Lineweight == 50);
  CHECK(part.Shadow == 1);
}

TEST_CASE("line: each release keeping what it has of the entity part")
{
  for (const Version release : std::array{Version::R2000, Version::R2004, Version::R2010})
  {
    CAPTURE(release);
    const Line taken = Replayed<Line>(Drawn("2DB", release), release);
    CHECK(taken.Record.Rgb.has_value() == (release >= Version::R2004));
    CHECK(taken.Record.Hidden);
  }
}

TEST_CASE("line: its thickness before its ends, its extrusion after")
{
  const Line thick = Replayed<Line>(Drawn("2CF"));
  CHECK(thick.Thickness == 5);
  CHECK(thick.Record.Lineweight == 35);
  const Line turned = Replayed<Line>(Drawn("2DA"));
  CHECK(turned.Extrusion == Vector{.X = 0, .Y = 1, .Z = 0});
}

TEST_CASE("line: R12's, without subclasses")
{
  const Line taken = Replayed<Line>(Drawn("2CF", Version::R12), Version::R12);
  CHECK(taken.Record.Color == 1);
  CHECK(taken.Thickness == 5);
}

TEST_CASE("line: R14's scale and visibility, which AutoCAD no longer writes")
{
  constexpr std::string_view text = "  0\r\nLINE\r\n  5\r\n2A\r\n330\r\n1F\r\n"
                                    "100\r\nAcDbEntity\r\n  8\r\n0\r\n 48\r\n3.0\r\n 60\r\n     1\r\n"
                                    "100\r\nAcDbLine\r\n 10\r\n0.0\r\n 20\r\n0.0\r\n 30\r\n0.0\r\n"
                                    " 11\r\n1.0\r\n 21\r\n1.0\r\n 31\r\n0.0\r\n";
  const Line taken = Replayed<Line>(std::string{text}, Version::R14);
  CHECK(taken.Record.Scale == 3);
  CHECK(taken.Record.Hidden);
}

TEST_CASE("line: a null handle given before its own")
{
  const auto all = Tags::All("  0\nLINE\n  5\n0\n  5\n2B\n");
  REQUIRE(all);
  File::Cursor in{*all};
  const auto one = in.Take();
  REQUIRE(one);
  std::vector<Issue> issues;
  const Line taken =
      DxfIO::Taken<Line>(*one, Fields::Reading{.Release = Version::R12, .Text = Encoding::Utf8(), .Issues = &issues});
  CHECK(taken.Record.Base.Self == Handle{0x2b});
}
} // namespace DxfIO::Entities
