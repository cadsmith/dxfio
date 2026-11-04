#include "objects/shapes/look.hpp"

#include "../alike.hpp"
#include "../entities/drawn.hpp"
#include "../kind.hpp"
#include "../releases.hpp"
#include "objects/io.hpp"

#include <string>

namespace DxfIO::Objects
{
namespace
{
auto Alikened(Version release = Version::R2018) -> VisualStyle
{
  const std::string record = Entities::Drawn("F5", release, "kinds");
  const VisualStyle out = DxfIO::Read<VisualStyle>(record, release);
  CHECK(out.Base.Extra.empty());
  CHECK(Alike(Written(out, release), record));
  return out;
}
} // namespace

TEST_CASE("visual style: its name and kind, what follows kept in order")
{
  const VisualStyle one = Alikened();
  CHECK(one.Name == "2dWireframe");
  CHECK(one.Kind == 4);
  CHECK(one.Version == 3);
  CHECK(one.Internal == false);
  REQUIRE_FALSE(one.Properties.empty());
  CHECK(one.Properties.front() == DxfIO::Group{.Code = 70, .Value = std::int64_t{58}});
}

TEST_CASE("visual style: r2010 flags each field, r2007 lists them bare")
{
  const VisualStyle flagged = Alikened(Version::R2010);
  CHECK(flagged.Version == 2);
  CHECK(flagged.Properties.at(1).Code == 176);
  const VisualStyle bare = Alikened(Version::R2007);
  CHECK_FALSE(bare.Version);
  CHECK_FALSE(bare.Internal);
  CHECK(bare.Properties.front().Code == 71);
}

TEST_CASE("visual style: every release alike")
{
  for (const Version release : Releases)
  {
    CAPTURE(release);
    CHECK(Alikened(release).Name == "2dWireframe");
  }
}

TEST_CASE("visual style: named as DXF names it")
{
  CHECK(Objects::Type(Object{VisualStyle{}}) == "VISUALSTYLE");
}
} // namespace DxfIO::Objects
