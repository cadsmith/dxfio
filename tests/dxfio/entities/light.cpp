#include "entities/shapes/light.hpp"

#include "../alike.hpp"
#include "../kind.hpp"
#include "../releases.hpp"
#include "drawn.hpp"
#include "entities/io.hpp"

#include "dxfio/colors.hpp"
#include "dxfio/entities/line.hpp"

#include <string>
#include <string_view>

namespace DxfIO::Entities
{
  namespace
  {
    auto Alikened(std::string_view self, Version release = Version::R2018) -> Light
    {
      const std::string record = Drawn(self, release, "kinds");
      const Light       out    = DxfIO::Read<Light>(record, release);
      CHECK(out.Record.Base.Extra.empty());
      CHECK(Alike(Written(out, release), record));
      return out;
    }
  } // namespace

  TEST_CASE("light: a point, a spot and a distant one, each its own kind")
  {
    const Light point = Alikened("31D");
    CHECK(point.Lamp->Name     == "Pointlight1");
    CHECK(point.Lamp->Kind     == 2);
    CHECK(point.Lamp->Position == Vector{.X = 260, .Y = 0, .Z = 5});
    CHECK(point.Lamp->Target   == Vector{.X = 260, .Y = 0, .Z = -5});
    CHECK(point.Lamp->Lit);
    CHECK(point.Lamp->Color == 7);
    CHECK(point.Lamp->Rgb   == Lamplight);
    CHECK_FALSE(point.Lamp->Glyph);
    CHECK(point.Lamp->Intensity == 1);
    CHECK_FALSE(point.Lamp->Limited);
    CHECK(point.Lamp->Hotspot == 45);
    CHECK(point.Lamp->Falloff == 50);
    CHECK(point.Lamp->Casting);
    CHECK(point.Lamp->Size           == 256);
    CHECK(Alikened("320").Lamp->Kind == 3);
    CHECK(Alikened("323").Lamp->Kind == 1);
  }

  TEST_CASE("light: every release alike")
  {
    for (const Version release : Releases)
    {
      CAPTURE(release);
      CHECK(Alikened("31D", release).Lamp->Name == "Pointlight1");
      CHECK(Alikened("320", release).Lamp->Kind == 3);
      CHECK(Alikened("323", release).Lamp->Kind == 1);
    }
  }

  TEST_CASE("light: AutoCAD's system layer, starred from r2007 only")
  {
    CHECK(Alikened("31D").Record.Layer == "*ADSK_SYSTEM_LIGHTS");
    CHECK(Alikened("31D", Version::R2004).Record.Layer == "ADSK_SYSTEM_LIGHTS");
  }

  TEST_CASE("light: r2000 packs its color in one group, written back the same")
  {
    const Light old = Alikened("31D", Version::R2000);
    CHECK(old.Lamp->Rgb == Lamplight);
    CHECK(Written(old, Version::R2000).find("-1023410177") != std::string::npos);
    Light red{};
    red.Lamp->Rgb = 0xFF0000;
    CHECK(DxfIO::Read<Light>(Written(red, Version::R2000), Version::R2000).Lamp->Rgb == 0xFF0000);
  }

  TEST_CASE("light: a packed color not given leaves the fresh one")
  {
    std::string       text = Written(Light{}, Version::R2000);
    const std::size_t from = text.find(" 90\r\n", text.find("  1\r\n"));
    REQUIRE(from != std::string::npos);
    text.erase(from, text.find("291\r\n", from) - from);
    CHECK(DxfIO::Read<Light>(text, Version::R2000).Lamp->Rgb == Lamplight);
  }

  TEST_CASE("light: alike only with what it holds alike")
  {
    Light one{};
    CHECK(one == Light{});
    one.Lamp->Size = 512;
    CHECK(one != Light{});
  }

  TEST_CASE("light: named as DXF names it")
  {
    CHECK(Named(Entity{Light{}}) == "LIGHT");
  }

  TEST_CASE("light: white in true color hides no color index white")
  {
    using namespace DxfIO::Colors;
    Line line{};
    line.Record.Color = White;
    CHECK(line.Record.Color == 7);
  }
} // namespace DxfIO::Entities
