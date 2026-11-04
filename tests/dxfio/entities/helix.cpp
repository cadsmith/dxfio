#include "entities/shapes/helix.hpp"

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
auto Sampled(Version release = Version::R2018) -> std::string
{
  return Drawn("2D8", release, "kinds");
}

auto Alikened(const std::string &record, Version release = Version::R2018) -> Helix
{
  const Helix out = DxfIO::Read<Helix>(record, release);
  CHECK(out.Record.Base.Extra.empty());
  CHECK(Alike(Written(out, release), record));
  return out;
}
} // namespace

TEST_CASE("helix: AutoCAD's cone, its start the base, 40 the top radius")
{
  const Helix one = Alikened(Sampled());
  CHECK(one.Curve->Degree == 3);
  CHECK(one.Curve->Knots.size() == 58);
  CHECK(one.Curve->Poles.size() == 54);
  CHECK(one.Coil.Major == 33);
  CHECK(one.Coil.Base == Vector{.X = 80});
  CHECK(one.Coil.Start == Vector{.X = 85});
  CHECK(one.Coil.Axis == Up);
  CHECK(one.Coil.Radius == 3);
  CHECK(one.Coil.Turns == 3);
  CHECK(one.Coil.Rise == doctest::Approx(10.0 / 3));
  CHECK(one.Coil.Right);
  CHECK(one.Coil.Constraint == 1);
  CHECK_FALSE(one.Picture.empty());
}

TEST_CASE("helix: every release alike")
{
  for (const Version release : Releases)
  {
    CAPTURE(release);
    CHECK(Alikened(Sampled(release), release).Coil.Turns == 3);
  }
}

TEST_CASE("helix: a fresh one stands up, one turn around z")
{
  const Helix one{};
  CHECK(one.Coil.Axis == Up);
  CHECK(one.Coil.Right);
  CHECK(one.Coil.Turns == 1);
}

TEST_CASE("helix: named as DXF names it")
{
  CHECK(Named(Entity{Helix{}}) == "HELIX");
}
} // namespace DxfIO::Entities
