#include "dxfio/appearance.hpp"

#include "dxfio/create.hpp"
#include "dxfio/edit/add.hpp"
#include "dxfio/view.hpp"

#include <doctest/doctest.h>

namespace DxfIO
{
  namespace
  {
    auto Walls() -> Drawing
    {
      Drawing       drawing = Create();
      Tables::Layer walls{};
      walls.Record.Name = "Walls";
      walls.Color       =         -3;
      walls.Rgb         = 0x00FF0000;
      walls.Linetype    = Symbol{"Dashed"};
      walls.Lineweight  = Lineweight::Mm050;
      REQUIRE(Edit::Add(drawing, walls));
      return drawing;
    }
  } // namespace

  TEST_CASE("appearance: ByLayer takes the layer's, its color even when the layer is off")
  {
    const Drawing    drawing = Walls();
    Entities::Record record{};
    record.Layer          = Symbol{"WALLS"};
    const Appearance look = AppearanceOf(drawing, record, nullptr);
    CHECK(look.Layer      == Symbol{"WALLS"});
    CHECK(look.Color      == 3);
    CHECK(look.Rgb        == 0x00FF0000);
    CHECK(look.Linetype   == Symbol{"Dashed"});
    CHECK(look.Lineweight == Lineweight::Mm050);
  }

  TEST_CASE("appearance: ByBlock takes the insert's, and outside a block the defaults")
  {
    const Drawing    drawing = Walls();
    Entities::Record record{};
    record.Color           = Colors::ByBlock;
    record.Linetype        = Symbol{"BYBLOCK"};
    record.Lineweight      = Lineweight::ByBlock;
    const Appearance alone = AppearanceOf(drawing, record, nullptr);
    CHECK(alone.Color == Colors::White);
    CHECK_FALSE(alone.Rgb);
    CHECK(alone.Linetype   == Symbol{"Continuous"});
    CHECK(alone.Lineweight == Lineweight::Default);

    const Appearance outer{.Layer      = Symbol{"Walls"},
                           .Color      =          5,
                           .Rgb        = 0x000000FF,
                           .Linetype   = Symbol{"Center"},
                           .Lineweight = Lineweight::Mm100};
    const Appearance held = AppearanceOf(drawing, record, &outer);
    CHECK(held.Layer      == Symbol{"Walls"});
    CHECK(held.Color      == 5);
    CHECK(held.Rgb        == 0x000000FF);
    CHECK(held.Linetype   == Symbol{"Center"});
    CHECK(held.Lineweight == Lineweight::Mm100);
  }

  TEST_CASE("appearance: an entity of a block on layer 0 is drawn on the insert's layer")
  {
    const Drawing    drawing = Walls();
    Entities::Record record{};
    const Appearance outer{.Layer = Symbol{"Walls"}};
    const Appearance look = AppearanceOf(drawing, record, &outer);
    CHECK(look.Layer    == Symbol{"Walls"});
    CHECK(look.Color    == 3);
    CHECK(look.Linetype == Symbol{"Dashed"});
    record.Layer = Symbol{"0"};
    CHECK(AppearanceOf(drawing, record, nullptr).Layer == Symbol{"0"});
    const Appearance other{.Layer = Symbol{"Elsewhere"}};
    record.Layer = Symbol{"Walls"};
    CHECK(AppearanceOf(drawing, record, &other).Layer == Symbol{"Walls"});
  }

  TEST_CASE("appearance: its own values stay, and a layer not in the table gives the defaults")
  {
    const Drawing    drawing = Walls();
    Entities::Record record{};
    record.Layer          = Symbol{"Nowhere"};
    record.Color          = 2;
    record.Linetype       = Symbol{"Hidden"};
    record.Lineweight     = Lineweight::Mm025;
    const Appearance look = AppearanceOf(drawing, record, nullptr);
    CHECK(look.Color == 2);
    CHECK_FALSE(look.Rgb);
    CHECK(look.Linetype   == Symbol{"Hidden"});
    CHECK(look.Lineweight == Lineweight::Mm025);
    record.Linetype          = Symbol{"ByLayer"};
    record.Color             = Colors::ByLayer;
    const Appearance missing = AppearanceOf(drawing, record, nullptr);
    CHECK(missing.Color    == Colors::White);
    CHECK(missing.Linetype == Symbol{"Continuous"});
  }
} // namespace DxfIO
