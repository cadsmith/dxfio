#include "../fields/fixture.hpp"
#include "file/sections/nests.hpp"
#include "tables/io.hpp"
#include "tags/tags.hpp"

#include "dxfio/drawing.hpp"

#include <doctest/doctest.h>

#include <string>
#include <string_view>
#include <vector>

namespace DxfIO::Tables
{
  namespace
  {
    constexpr std::string_view Text =
        "  0\nTABLE\n  2\nWEIRD\n  0\nENDTAB\n"
        "  0\nTABLE\n  2\nLAYER\n  5\n2\n330\n0\n100\nAcDbSymbolTable\n 70\n9\n"
        "  0\nLAYER\n  5\n10\n330\n2\n100\nAcDbSymbolTableRecord\n"
        "100\nAcDbLayerTableRecord\n  2\n0\n 70\n0\n 62\n7\n  6\nContinuous\n"
        "370\n-3\n  0\nENDTAB\n"
        "  0\nTABLE\n  2\nDIMSTYLE\n  5\nA\n330\n0\n100\nAcDbSymbolTable\n"
        " 70\n0\n  0\nENDTAB\n"
        "  0\nTABLE\n  2\nVPORT\n  5\n8\n330\n0\n100\nAcDbSymbolTable\n 70\n0\n"
        "  0\nENDTAB\n";

    auto Read(std::string_view all) -> Drawing
    {
      const auto found = Tags::All(all);
      REQUIRE(found);
      const auto nests = File::Sections::Tables(*found);
      REQUIRE(nests);
      Drawing            drawing{};
      std::vector<Issue> issues;
      Tables::Read(*nests, drawing, Fields::Reading{.Issues = &issues});
      return drawing;
    }
  } // namespace

  TEST_CASE("tables: a kind's table into its member, others kept unknown")
  {
    const Drawing drawing = Read(Text);
    CHECK(drawing.Layer.Control.Name     == "LAYER");
    CHECK(drawing.Layer.Control.Count    == 9);
    REQUIRE(drawing.Layer.Records.size() == 1);
    CHECK(drawing.Layer.Records.front().Record.Name == "0");
    CHECK(drawing.VPort.Control.Name    == "VPORT");
    CHECK(drawing.DimStyle.Control.Name == "DIMSTYLE");
    CHECK(drawing.Loose.size()          == 1);
    CHECK(drawing.Style.Control.Name.empty());
  }

  TEST_CASE("tables: a table given again kept out, the first kept and told")
  {
    const auto found = Tags::All("  0\nTABLE\n  2\nLAYER\n 70\n1\n  0\nENDTAB\n"
                                 "  0\nTABLE\n  2\nLAYER\n 70\n2\n  0\nENDTAB\n");
    REQUIRE(found);
    const auto nests = File::Sections::Tables(*found);
    REQUIRE(nests);
    Drawing            drawing{};
    std::vector<Issue> issues;
    Tables::Read(*nests, drawing, Fields::Reading{.Issues = &issues});
    CHECK(drawing.Layer.Control.Count == 1);
    REQUIRE(issues.size()             == 1);
    CHECK(issues.front().Line         == 9);
    CHECK(issues.front().What         == "table LAYER given again, the first kept");
  }

  TEST_CASE("tables: a record not of its table's kind left out and told, a stranger's kept")
  {
    const auto found = Tags::All("  0\nTABLE\n  2\nLAYER\n 70\n2\n"
                                 "  0\nFOO\n  2\nodd\n"
                                 "  0\nLAYER\n  2\n0\n 70\n0\n 62\n7\n  6\nContinuous\n"
                                 "  0\nENDTAB\n"
                                 "  0\nTABLE\n  2\nWEIRD\n  0\nFOO\n  2\nkept\n  0\nENDTAB\n");
    REQUIRE(found);
    const auto nests = File::Sections::Tables(*found);
    REQUIRE(nests);
    Drawing            drawing{};
    std::vector<Issue> issues;
    Tables::Read(*nests, drawing, Fields::Reading{.Release = Version::R12, .Issues = &issues});
    REQUIRE(drawing.Layer.Records.size()            == 1);
    CHECK(drawing.Layer.Records.front().Record.Name == "0");
    REQUIRE(drawing.Loose.size() == 1);
    CHECK(drawing.Loose.front().Records.size() == 1);
    REQUIRE(issues.size()     == 1);
    CHECK(issues.front().Line == 7);
    CHECK(issues.front().What == "record FOO in table LAYER, left out");
  }

  TEST_CASE("tables: written in AutoCAD's order, its name before its handle")
  {
    Fields::Sheet page;
    REQUIRE(Write(page.To, Read(Text)));
    const std::string &out = page.Out;
    CHECK(out.find("VPORT") < out.find("LAYER"));
    CHECK(out.find("LAYER") < out.find("DIMSTYLE"));
    CHECK(out.find("DIMSTYLE") < out.find("WEIRD"));
    CHECK(out.find("  0\r\nTABLE\r\n  2\r\nLAYER\r\n  5\r\n2\r\n330\r\n0\r\n"
                   "100\r\nAcDbSymbolTable\r\n 70\r\n     9\r\n")
          != std::string::npos);
    CHECK(out.find("\nSTYLE\r") == std::string::npos);
  }

  TEST_CASE("tables: a stray table named as a kind is not written twice")
  {
    Drawing drawing = Read(Text);
    drawing.Loose.push_back(Table<Unknown>{
        .Control = Tables::Control{.Name = "LAYER"},
        .Records = {},
    });
    Fields::Sheet page{};
    REQUIRE(Tables::Write(page.To, drawing));
    const std::size_t first = page.Out.find("LAYER");
    REQUIRE(first != std::string::npos);
    const std::size_t table = page.Out.find("\r\nTABLE\r\n", first);
    CHECK((table == std::string::npos || page.Out.find("LAYER", table) == std::string::npos));
  }
} // namespace DxfIO::Tables
