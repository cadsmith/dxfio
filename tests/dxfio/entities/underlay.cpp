#include "entities/shapes/underlay.hpp"

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
template <class KindT>
auto Alikened(std::string_view self, Version release = Version::R2018) -> KindT
{
  const std::string record = Drawn(self, release, "kinds");
  const KindT out = DxfIO::Read<KindT>(record, release);
  CHECK(out.Record.Base.Extra.empty());
  CHECK(Alike(Written(out, release), record));
  return out;
}
} // namespace

TEST_CASE("underlay: a pdf, its defaults left out as AutoCAD leaves them")
{
  const PdfUnderlay one = Alikened<PdfUnderlay>("326");
  CHECK(one.Overlaid->Definition == Handle{.Value = 0x325});
  CHECK(one.Overlaid->Position == Vector{.X = 300});
  CHECK(one.Overlaid->Scale == Vector{.X = 1, .Y = 1, .Z = 1});
  CHECK(one.Overlaid->Rotation == 0);
  CHECK(one.Overlaid->Extrusion == Up);
  CHECK(one.Overlaid->Flags == Visible);
  CHECK(one.Overlaid->Contrast == Whole);
  CHECK(one.Overlaid->Fade == 0);
  CHECK(one.Overlaid->Clip.empty());
}

TEST_CASE("underlay: every release alike, each of the three")
{
  for (const Version release : Releases)
  {
    CAPTURE(release);
    CHECK(Alikened<PdfUnderlay>("326", release).Overlaid->Position.X == 300);
    CHECK(Alikened<DwfUnderlay>("32A", release).Overlaid->Position.X == 320);
    CHECK(Alikened<DgnUnderlay>("32D", release).Overlaid->Position.X == 340);
  }
}

TEST_CASE("underlay: scaled, turned and clipped, each kept")
{
  DwfUnderlay one{};
  one.Record.Base.Self = Handle{.Value = 0x2a};
  one.Overlaid->Definition = Handle{.Value = 0x2b};
  one.Overlaid->Scale = Vector{.X = 2, .Y = 3, .Z = 4};
  one.Overlaid->Rotation = 30;
  one.Overlaid->Extrusion = Vector{.X = 1};
  one.Overlaid->Clip = {Bound{.Position = Vector{.X = 1, .Y = 2}}, Bound{.Position = Vector{.X = 3, .Y = 4}}};
  const DwfUnderlay back = DxfIO::Read<DwfUnderlay>(Written(one));
  CHECK(back == one);
  CHECK(Written(one).find(" 31\r\n") == std::string::npos);
}

TEST_CASE("underlay: named as DXF names them")
{
  CHECK(Named(Entity{PdfUnderlay{}}) == "PDFUNDERLAY");
  CHECK(Named(Entity{DwfUnderlay{}}) == "DWFUNDERLAY");
  CHECK(Named(Entity{DgnUnderlay{}}) == "DGNUNDERLAY");
}
} // namespace DxfIO::Entities
