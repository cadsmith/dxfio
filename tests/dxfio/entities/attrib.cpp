#include "entities/shapes/attrib.hpp"

#include "../alike.hpp"
#include "../kind.hpp"
#include "drawn.hpp"

#include <string>
#include <string_view>

namespace DxfIO::Entities
{
  namespace
  {
    auto Marked(std::string_view self, Version release = Version::R2018) -> std::string
    {
      return Drawn(self, release, "inserts");
    }

    constexpr std::string_view Opening =
        "  0\r\nATTRIB\r\n  5\r\n2A\r\n330\r\n1F\r\n100\r\nAcDbEntity\r\n  "
        "8\r\n0\r\n"
        "100\r\nAcDbText\r\n 10\r\n0.0\r\n 20\r\n0.0\r\n 30\r\n0.0\r\n"
        " 40\r\n1.0\r\n  1\r\nx\r\n100\r\nAcDbAttribute\r\n";

    auto Alikened(const std::string &text, Version release = Version::R2018) -> Attrib
    {
      const Attrib out = Read<Attrib>(text, release);
      CHECK(out.Record.Base.Extra.empty());
      CHECK(Alike(Written(out, release), text));
      return out;
    }

    auto Joined(std::string_view rest) -> std::string
    {
      return std::string{Opening} + std::string{rest};
    }
  } // namespace

  TEST_CASE("attrib: a plain one, its text and its tag")
  {
    const Attrib taken = Replayed<Attrib>(Marked("2E3"));
    CHECK(taken.Lettering.Value    == "first value");
    CHECK(taken.Lettering.Position == Vector{.X = 0, .Y = 21, .Z = 0});
    CHECK(taken.Lettering.Height   == 0.5);
    CHECK(taken.Tag      == "ONE");
    CHECK(taken.Flags    == 0);
    CHECK(taken.Revision == 0);
    CHECK_FALSE(taken.Locked);
  }

  TEST_CASE("attrib: styled and aligned, its 74 past its flags")
  {
    const Attrib taken = Alikened(Marked("2E4"));
    CHECK(taken.Lettering.Style      == "tall");
    CHECK(taken.Lettering.Width      == 0.8);
    CHECK(taken.Lettering.Horizontal == 1);
    CHECK(taken.Lettering.Alignment  == Vector{.X = 1, .Y = 22, .Z = 0});
    CHECK(taken.Flags    == 8);
    CHECK(taken.Vertical == 2);
  }

  TEST_CASE("attrib: thick and mirrored, on another plane")
  {
    const Attrib thick = Replayed<Attrib>(Marked("2FC"));
    CHECK(thick.Lettering.Thickness         == 1.5);
    CHECK(thick.Lettering.Generation        == 2);
    CHECK(thick.Record.Base.Extended.size() == 1);
    const Attrib flipped = Replayed<Attrib>(Marked("2FD"));
    CHECK(flipped.Lettering.Generation  == 4);
    CHECK(flipped.Lettering.Extrusion.Z == -1);
    CHECK(flipped.Flags == 4);
  }

  TEST_CASE("attrib: with no AcDbText its fields found all the same")
  {
    const Attrib taken =
        DxfIO::Read<Attrib>("  0\r\nATTRIB\r\n  5\r\n2A\r\n100\r\nAcDbEntity\r\n  8\r\n0\r\n"
                            "100\r\nAcDbAttribute\r\n 10\r\n1.0\r\n 20\r\n2.0\r\n 30\r\n0.0\r\n"
                            " 40\r\n2.0\r\n  1\r\nv\r\n  2\r\nT\r\n 70\r\n     0\r\n");
    CHECK(taken.Lettering.Value  == "v");
    CHECK(taken.Lettering.Height == 2);
    CHECK(taken.Tag == "T");
  }

  TEST_CASE("attrib: in paper space with its INSERT")
  {
    CHECK(Replayed<Attrib>(Marked("318")).Record.Paper);
  }

  TEST_CASE("attrib: its 280s by release, none, a lock, then a version too")
  {
    for (const Version release : Releases)
    {
      CAPTURE(release);
      const std::string text  = Marked("2E4", release);
      const std::size_t count = release < Version::R2007 ? 0 : (release < Version::R2010 ? 1 : 2);
      std::size_t       held  = 0;
      for (std::size_t at = text.find("280\r\n"); at != std::string::npos;
           at             = text.find("280\r\n", at + 1))
      {
        ++held;
      }
      CHECK(held == count);
      CHECK(Alikened(text, release).Tag == "TWO");
    }
  }

  TEST_CASE("attrib: the first 280 its version, the last its lock")
  {
    const Attrib taken =
        Replayed<Attrib>(Joined("280\r\n     0\r\n  2\r\nT\r\n 70\r\n     0\r\n280\r\n     1\r\n"));
    CHECK(taken.Revision == 0);
    CHECK(taken.Locked);
  }

  TEST_CASE("attrib: a 280 alone is its lock, written after a version of 0")
  {
    const Attrib taken = Read<Attrib>(Joined("  2\r\nT\r\n 70\r\n     0\r\n280\r\n     1\r\n"));
    CHECK(taken.Revision == 0);
    CHECK(taken.Locked);
    CHECK(Written(taken).contains("280\r\n     0\r\n  2\r\nT\r\n"));
  }

  TEST_CASE("attrib: a 280 alone of 0, and none, unlocked")
  {
    const Attrib zero = Read<Attrib>(Joined("  2\r\nT\r\n 70\r\n     0\r\n280\r\n     0\r\n"));
    CHECK_FALSE(zero.Locked);
    const Attrib none = Read<Attrib>(Joined("  2\r\nT\r\n 70\r\n     0\r\n"));
    CHECK_FALSE(none.Locked);
  }

  TEST_CASE("attrib: R2007's one 280 its lock, as AutoCAD writes it")
  {
    const Attrib taken =
        Replayed<Attrib>(Joined("  2\r\nT\r\n 70\r\n     0\r\n280\r\n     1\r\n"), Version::R2007);
    CHECK(taken.Locked);
    CHECK(taken.Revision == 0);
  }

  TEST_CASE("attrib: a new one writes both 280s")
  {
    const std::string text = Written(Attrib{});
    CHECK(text.contains("100\r\nAcDbAttribute\r\n280\r\n     0\r\n  2\r\n"));
    CHECK(text.ends_with(" 70\r\n     0\r\n280\r\n     0\r\n"));
  }
} // namespace DxfIO::Entities
