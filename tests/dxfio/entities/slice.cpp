#include "entities/shapes/slice.hpp"

#include "../alike.hpp"
#include "../kind.hpp"
#include "../releases.hpp"
#include "drawn.hpp"
#include "entities/io.hpp"

#include <string>
#include <string_view>

namespace DxfIO::Entities
{
  namespace
  {
    auto Alikened(Version release = Version::R2018) -> SectionObject
    {
      const std::string   record = Drawn("314", release, "kinds");
      const SectionObject out    = DxfIO::Read<SectionObject>(record, release);
      CHECK(out.Record.Base.Extra.empty());
      CHECK(Alike(Written(out, release), record));
      return out;
    }
  } // namespace

  TEST_CASE("slice: AutoCAD's section plane, its line and its settings")
  {
    const SectionObject one = Alikened();
    CHECK(one.Data->State    == 1);
    CHECK(one.Data->Flags    == 5);
    CHECK(one.Data->Name     == "Section Plane (1)");
    CHECK(one.Data->Vertical == Up);
    CHECK(one.Data->Top > one.Data->Bottom);
    CHECK(one.Data->Transparency == 70);
    CHECK(one.Data->Color        == 9);
    CHECK_FALSE(one.Data->Rgb);
    REQUIRE(one.Data->Vertices.size() == 2);
    CHECK(one.Data->Vertices.back()   == Vector{.X = 250});
    CHECK(one.Data->Back.empty());
    CHECK(one.Data->Settings == Handle{.Value = 0x316});
    CHECK_FALSE(one.Picture.empty());
    CHECK_FALSE(one.Record.Base.Extended.empty());
  }

  TEST_CASE("slice: every release alike")
  {
    for (const Version release : Releases)
    {
      CAPTURE(release);
      CHECK(Alikened(release).Data->Vertices.size() == 2);
    }
  }

  TEST_CASE("slice: a true color and a back line, each kept")
  {
    SectionObject one{};
    one.Record.Base.Self     = Handle{.Value = 0x2a};
    one.Data->Name            = "A";
    one.Data->Rgb             = 255;
    one.Data->Vertices        = {Vector{.X = 1}, Vector{.X = 2}};
    one.Data->Back            = {Vector{.Y = 3}};
    const SectionObject back = DxfIO::Read<SectionObject>(Written(one));
    CHECK(back.Data->Rgb  == 255);
    CHECK(back.Data->Back == one.Data->Back);
    CHECK(back           == one);
  }

  TEST_CASE("slice: named as DXF names it, not as a file's sections are")
  {
    CHECK(Named(Entity{SectionObject{}}) == "SECTIONOBJECT");
  }
} // namespace DxfIO::Entities
