#include "empty/styles.hpp"

#include "empty/roots.hpp"

#include <doctest/doctest.h>

namespace DxfIO::Empty
{
  TEST_CASE("styles: the materials, Global's maps placed as AutoCAD places them")
  {
    Seed                                 handles;
    const Roots                          ids = Taken(handles);
    const std::vector<Objects::Material> all = Materials(ids);
    REQUIRE(all.size()         == 3);
    CHECK(all.front().Name     == "ByBlock");
    CHECK(all.at(1).Base.Self  == ids.Materials.Bylayer);
    CHECK(all.front().Channels == 63);
    const Objects::Material &global = all.back();
    CHECK(global.Maps->Diffuse.Transform.Cells.front() == 0.0007999999797903);
    CHECK(global.Maps->Bump.Transform     == global.Maps->Diffuse.Transform);
    CHECK(global.Maps->Specular.Transform == Identity);
  }

  TEST_CASE("styles: Standard's of multileaders, multilines and tables")
  {
    Seed                        handles;
    const Roots                 ids    = Taken(handles);
    const Objects::MLeaderStyle leader = Mleaderstyle(ids, handles);
    CHECK(leader.Leader->Linetype == ids.Linetype);
    CHECK(leader.Text->Style      == ids.Style);
    const Objects::MLineStyle line = Mlinestyle(handles);
    REQUIRE(line.Elements.size()      == 2);
    CHECK(line.Elements.back().Offset == -0.5);
    const Objects::TableStyle table = Tablestyle(handles);
    REQUIRE(table.Cells.size()          == 3);
    CHECK(table.Cells.at(1).Height      == 6);
    CHECK(table.Cells.front().Alignment == 2);
  }
} // namespace DxfIO::Empty
