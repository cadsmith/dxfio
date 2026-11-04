#include "empty/layouts.hpp"

#include "empty/roots.hpp"
#include "empty/space.hpp"

#include <doctest/doctest.h>

namespace DxfIO::Empty
{
TEST_CASE("layout: named, on its handles, plotted nowhere")
{
  const Objects::Layout sheet = Layout("Layout2", Handle{.Value = 0x30}, Handle{.Value = 0x31});
  CHECK(sheet.Name == "Layout2");
  CHECK(sheet.Base.Self == Handle{.Value = 0x30});
  CHECK(sheet.Block == Handle{.Value = 0x31});
  CHECK(sheet.Plot->Device == "none_device");
  CHECK(sheet.Limmax == Objects::Limits);
  CHECK(sheet.Order == 0);
  CHECK_FALSE(sheet.Base.Owner);
}

TEST_CASE("layouts: Layout1 then Model, each of its space, plotted nowhere")
{
  Seed handles;
  const Roots ids = Taken(handles);
  const Handle active{.Value = 0x99};
  const std::vector<Objects::Layout> all = Layouts(ids, active);
  REQUIRE(all.size() == 2);
  const Objects::Layout &paper = all.front();
  CHECK(paper.Name == "Layout1");
  CHECK(paper.Order == 1);
  CHECK(paper.Base.Self == ids.Layouts.Paper);
  CHECK(paper.Block == ids.Records.Paper);
  CHECK_FALSE(paper.Viewport);
  CHECK(paper.Plot->Device == "none_device");
  const Objects::Layout &model = all.back();
  CHECK(model.Name == "Model");
  CHECK(model.Order == 0);
  CHECK(model.Block == ids.Records.Model);
  CHECK(model.Viewport == active);
  CHECK(model.Plot->Size == "ISO_A4_(210.00_x_297.00_MM)");
}
} // namespace DxfIO::Empty
