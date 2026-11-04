#include "entities/shapes/raster.hpp"

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
  const std::string record = Drawn(self, release, "rasters");
  const KindT out = DxfIO::Read<KindT>(record, release);
  CHECK(out.Record.Base.Extra.empty());
  CHECK(Alike(Written(out, release), record));
  return out;
}
} // namespace

TEST_CASE("raster: an IMAGE, its pixels and its definition, in every release")
{
  for (const Version release : Releases)
  {
    const Image one = Alikened<Image>("2D1", release);
    CHECK(one.Raster->Size == Vector{.X = 96, .Y = 96});
    CHECK(one.Raster->Across.X == doctest::Approx(10.0 / 96));
    CHECK(one.Raster->Kind == 1);
    REQUIRE(one.Raster->Corners.size() == 2);
    CHECK(one.Raster->Definition != Handle{});
  }
}

TEST_CASE("raster: an IMAGE turned")
{
  const Image one = Alikened<Image>("2D3");
  CHECK(one.Record.Layer.View() == "notes");
  CHECK(one.Raster->Down.X < 0);
}

TEST_CASE("raster: a WIPEOUT, its polygon, in every release")
{
  for (const Version release : Releases)
  {
    const Wipeout one = Alikened<Wipeout>("2D4", release);
    CHECK(one.Raster->Kind == 2);
    REQUIRE(one.Raster->Corners.size() == 5);
    CHECK(one.Raster->Corners.at(0).Position.X == -0.5);
    CHECK(one.Raster->Definition == Handle{});
  }
}

TEST_CASE("raster: a new one shown, clipped to a rectangle, its brightness and contrast as AutoCAD's")
{
  const Raster one{};
  CHECK(one.Flags == 7);
  CHECK(one.Brightness == 50);
  CHECK(one.Contrast == 50);
  CHECK(one.Fade == 0);
  CHECK(one.Kind == 1);
  const std::string text = Written(Image{});
  CHECK(text.contains(" 70\r\n     7\r\n280\r\n     0\r\n281\r\n    50\r\n282\r\n    50\r\n"));
  CHECK(text.contains(" 71\r\n     1\r\n"));
}

TEST_CASE("raster: a WIPEOUT of three sides, on paper")
{
  CHECK(Alikened<Wipeout>("2D6").Raster->Corners.size() == 4);
  CHECK(Alikened<Wipeout>("2E7").Record.Paper);
}
} // namespace DxfIO::Entities
