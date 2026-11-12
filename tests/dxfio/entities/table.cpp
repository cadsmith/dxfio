#include "entities/shapes/table.hpp"

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
    auto Alikened(std::string_view self, Version release = Version::R2018) -> AcadTable
    {
      const std::string record = Drawn(self, release, "schedules");
      const AcadTable   out    = DxfIO::Read<AcadTable>(record, release);
      CHECK(Alike(Written(out, release), record));
      return out;
    }
  } // namespace

  TEST_CASE("table: its block and point, its cells kept, in every release")
  {
    for (const Version release : Releases)
    {
      const AcadTable one = Alikened("2CD", release);
      CHECK(one.Block.View() == "*T1");
      CHECK(one.Position     == Vector{});
      CHECK(one.Record.Base.Extra.empty());
      CHECK_FALSE(one.Body.empty());
    }
  }

  TEST_CASE("table: turned and scaled in its block, on paper")
  {
    const AcadTable turned = Alikened("2E1");
    CHECK(turned.Position == Vector{.X = 100});
    CHECK(turned.Rotation == 0);
    CHECK(turned.Scale    == Vector{.X = 1, .Y = 1, .Z = 1});
    CHECK(Alikened("2FE").Record.Paper);
  }
} // namespace DxfIO::Entities
