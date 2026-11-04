#include "entities/shapes/shape.hpp"

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
auto Alikened(std::string_view self, Version release = Version::R2018) -> Shape
{
  const std::string record = Drawn(self, release, "symbols");
  const Shape out = DxfIO::Read<Shape>(record, release);
  CHECK(out.Record.Base.Extra.empty());
  CHECK(Alike(Written(out, release), record));
  return out;
}
} // namespace

TEST_CASE("shape: a plain one, what a new one holds left out")
{
  const Shape one = Alikened("2CF");
  CHECK(one.Name.View() == "RES");
  CHECK(one.Size == 2);
  CHECK(one.Width == 1);
  CHECK(one.Extrusion == Up);
}

TEST_CASE("shape: every group, in every release")
{
  for (const Version release : Releases)
  {
    const Shape one = Alikened("2D0", release);
    CHECK(one.Record.Layer.View() == "notes");
    CHECK(one.Thickness == 4);
    CHECK(one.Rotation == doctest::Approx(45.0001));
    CHECK(one.Width == 2);
    CHECK(one.Oblique == doctest::Approx(15));
    CHECK(one.Extrusion.Y == doctest::Approx(0.6));
  }
}

TEST_CASE("shape: R12's, and on paper")
{
  CHECK(Alikened("2D1", Version::R12).Width == 0.5);
  CHECK(Alikened("2E7").Record.Paper);
}
} // namespace DxfIO::Entities
