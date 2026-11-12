#include "objects/shapes/settings.hpp"

#include "../fields/fixture.hpp"
#include "../kind.hpp"

#include <string>

namespace DxfIO::Objects
{
  TEST_CASE("plot settings: a page setup is a layout's plot and no more")
  {
    PlotSettings page{};
    page.Base.Self         = Handle{0x2A};
    page.Base.Owner        = Handle{0xC};
    page.Plot->Name        = "A1 sheet";
    const std::string text = Written(page);
    CHECK(text.contains("PLOTSETTINGS"));
    CHECK(text.contains("AcDbPlotSettings"));
    CHECK_FALSE(text.contains("AcDbLayout"));
    CHECK(Read<PlotSettings>(text) == page);
  }
} // namespace DxfIO::Objects
