#include "entities/shapes/modeler.hpp"

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

constexpr std::string_view Boxy = "2CE";

constexpr std::string_view Flat = "2D7";

constexpr std::string_view Sheeted = "2DA";

constexpr std::string_view Patch = "2EA";

auto Oldest(std::string_view type, std::string_view extra) -> std::string
{
  return "  0\r\n" + std::string{type} +
         "\r\n  5\r\n2A\r\n330\r\n1F\r\n100\r\nAcDbEntity\r\n  8\r\n0\r\n"
         "100\r\nAcDbModelerGeometry\r\n 70\r\n     1\r\n"
         "  1\r\nkoo nnm n o\r\n  3\r\nrest of it\r\n  1\r\nnext\r\n" +
         std::string{extra};
}
} // namespace

TEST_CASE("modeler: a solid's body in its lines until r2010, kept in order")
{
  const Solid3d one = Alikened<Solid3d>(Boxy, Version::R2007);
  CHECK(one.Geometry->Version == 1);
  CHECK_FALSE(one.Geometry->Lines.empty());
  CHECK(one.Geometry->Lines.front().Code == 1);
  CHECK_FALSE(one.Geometry->Stored);
  CHECK_FALSE(one.Geometry->Guid);
  CHECK(one.History != Handle{});
}

TEST_CASE("modeler: from r2013 the body is a guid, what it names kept apart")
{
  const Solid3d one = Alikened<Solid3d>(Boxy);
  CHECK(one.Geometry->Lines.empty());
  CHECK_FALSE(one.Geometry->Version);
  CHECK(one.Geometry->Stored == true);
  REQUIRE(one.Geometry->Guid);
  CHECK(one.Geometry->Guid->starts_with("{"));
  CHECK(one.History != Handle{});
}

TEST_CASE("modeler: no history before r2007, no subclass for one")
{
  const Solid3d one = Alikened<Solid3d>(Boxy, Version::R2000);
  CHECK(one.History == Handle{});
  CHECK(Written(one, Version::R2000).find("AcDb3dSolid") == std::string::npos);
}

TEST_CASE("modeler: AutoCAD refuses a solid of r2007 without a history group")
{
  const std::string text = Written(Solid3d{}, Version::R2007);
  CHECK(text.find("AcDb3dSolid\r\n350\r\n0\r\n") != std::string::npos);
}

TEST_CASE("modeler: every release alike, each kind")
{
  for (const Version release : Releases)
  {
    CAPTURE(release);
    CHECK(Alikened<Solid3d>(Boxy, release).Geometry != Boxed<Geometry>{});
    CHECK(Alikened<Region>(Flat, release).Geometry != Boxed<Geometry>{});
    CHECK(Alikened<PlaneSurface>(Sheeted, release).Across == 6);
    CHECK(Alikened<ExtrudedSurface>("332", release).Along == 6);
    CHECK_FALSE(Alikened<RevolvedSurface>("33E", release).Tail.empty());
    CHECK_FALSE(Alikened<LoftedSurface>("348", release).Tail.empty());
    CHECK_FALSE(Alikened<SweptSurface>("355", release).Tail.empty());
  }
}

TEST_CASE("modeler: r2013 names by nurbs what r2010 names by surface alone")
{
  CHECK(Alikened<Surface>(Patch, Version::R2010).Tail.empty());
  CHECK(Alikened<NurbSurface>(Patch, Version::R2013).Tail.front().Code == 100);
  CHECK(Alikened<NurbSurface>(Patch).Tail.front().Code == 100);
}

TEST_CASE("modeler: a surface's own subclass stays a run of groups, in order")
{
  const ExtrudedSurface one = Alikened<ExtrudedSurface>("332");
  REQUIRE_FALSE(one.Tail.empty());
  CHECK(one.Tail.front() == Group{.Code = 100, .Value = std::string{"AcDbExtrudedSurface"}});
  CHECK(one.Tail.back().Code == 11 + 20);
}

TEST_CASE("modeler: a body, kept like a region, its lines long and short")
{
  const Body one = Replayed<Body>(Oldest("BODY", ""), Version::R2000);
  REQUIRE(one.Geometry->Lines.size() == 3);
  CHECK(one.Geometry->Lines.at(1).Code == 3);
  CHECK(one.Geometry->Lines.back().Code == 1);
  CHECK(Replayed<Region>(Oldest("REGION", ""), Version::R2000).Picture.empty());
}

TEST_CASE("modeler: alike only with what it holds alike")
{
  Solid3d one{};
  Solid3d two = one;
  CHECK(one == two);
  two.History = Handle{.Value = 7};
  CHECK(one != two);
  Region flat{};
  CHECK(flat == Region{});
  Body body{};
  body.Picture = "x";
  CHECK(body != Body{});
  LoftedSurface sheet{};
  sheet.Across = 1;
  CHECK(sheet != LoftedSurface{});
}

TEST_CASE("modeler: named as DXF names them")
{
  CHECK(Named(Entity{Region{}}) == "REGION");
  CHECK(Named(Entity{Body{}}) == "BODY");
  CHECK(Named(Entity{Solid3d{}}) == "3DSOLID");
  CHECK(Named(Entity{Surface{}}) == "SURFACE");
  CHECK(Named(Entity{PlaneSurface{}}) == "PLANESURFACE");
  CHECK(Named(Entity{NurbSurface{}}) == "NURBSURFACE");
  CHECK(Named(Entity{ExtrudedSurface{}}) == "EXTRUDEDSURFACE");
  CHECK(Named(Entity{LoftedSurface{}}) == "LOFTEDSURFACE");
  CHECK(Named(Entity{RevolvedSurface{}}) == "REVOLVEDSURFACE");
  CHECK(Named(Entity{SweptSurface{}}) == "SWEPTSURFACE");
}
} // namespace DxfIO::Entities
