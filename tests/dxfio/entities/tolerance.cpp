#include "entities/shapes/tolerance.hpp"

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
    auto Alikened(std::string_view self, Version release = Version::R2018) -> Tolerance
    {
      const std::string record = Drawn(self, release, "symbols");
      const Tolerance   out    = DxfIO::Read<Tolerance>(record, release);
      CHECK(out.Record.Base.Extra.empty());
      CHECK(Alike(Written(out, release), record));
      return out;
    }
  } // namespace

  TEST_CASE("tolerance: a frame along x, in every release")
  {
    for (const Version release : Releases)
    {
      const Tolerance one = Alikened("2D2", release);
      CHECK(one.Style     == "ISO-25");
      CHECK(one.Direction == Vector{.X = 1});
      CHECK(one.Text.starts_with("{\\Fgdt;j}%%v"));
    }
  }

  TEST_CASE("tolerance: two rows, turned")
  {
    const Tolerance one = Alikened("2D5");
    CHECK(one.Direction.Y         == doctest::Approx(0.7071));
    CHECK(one.Record.Layer.View() == "notes");
  }

  TEST_CASE("tolerance: its normal down")
  {
    CHECK(Alikened("2D6").Normal.Z == -1);
  }

  TEST_CASE("tolerance: its style a symbol, as a layer is")
  {
    CHECK(std::is_same_v<decltype(Tolerance::Style), Symbol>);
  }
} // namespace DxfIO::Entities
