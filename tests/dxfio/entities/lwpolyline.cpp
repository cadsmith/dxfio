#include "entities/shapes/lwpolyline.hpp"

#include "../kind.hpp"
#include "drawn.hpp"

#include <initializer_list>
#include <string>
#include <string_view>

namespace DxfIO::Entities
{
  namespace
  {
    auto Traced(std::string_view self, Version release = Version::R2018) -> std::string
    {
      return Drawn(self, release, "polylines");
    }

    constexpr std::string_view Opening = "  0\r\nLWPOLYLINE\r\n  5\r\n2A\r\n330\r\n1F\r\n"
                                         "100\r\nAcDbEntity\r\n  8\r\n0\r\n100\r\nAcDbPolyline\r\n";

    constexpr std::string_view Origin = " 10\r\n0.0\r\n 20\r\n0.0\r\n";

    constexpr std::string_view Corner = " 10\r\n10.0\r\n 20\r\n0.0\r\n";

    constexpr std::string_view Top = " 10\r\n10.0\r\n 20\r\n10.0\r\n";

    auto Joined(std::initializer_list<std::string_view> parts) -> std::string
    {
      std::string out{Opening};
      for (const std::string_view part : parts)
      {
        out += part;
      }
      return out;
    }
  } // namespace

  TEST_CASE("lwpolyline: an open one, its width written at 0")
  {
    const LwPolyline taken = Replayed<LwPolyline>(Traced("2CD"));
    REQUIRE(taken.Vertices.size()       == 3);
    CHECK(taken.Vertices.at(1).Position == Vector{.X = 10, .Y = 0, .Z = 0});
    CHECK(taken.Flags == 0);
    CHECK(taken.Width == 0.0);
    CHECK_FALSE(taken.Vertices.at(1).Widths);
  }

  TEST_CASE("lwpolyline: closed, and its linetype generated")
  {
    CHECK(Replayed<LwPolyline>(Traced("2CE")).Flags == 1);
    CHECK(Replayed<LwPolyline>(Traced("2D6")).Flags == 128);
  }

  TEST_CASE("lwpolyline: one width for every segment, on no vertex")
  {
    const LwPolyline taken = Replayed<LwPolyline>(Traced("2CF"));
    CHECK(taken.Width == 2);
    CHECK_FALSE(taken.Vertices.front().Widths);
  }

  TEST_CASE("lwpolyline: widths of its own on every vertex, 0 too, no 43")
  {
    const LwPolyline taken = Replayed<LwPolyline>(Traced("2D0"));
    CHECK_FALSE(taken.Width);
    REQUIRE(taken.Vertices.size()     == 4);
    CHECK(taken.Vertices.at(0).Widths == Widths{.Start = 1, .End = 3});
    CHECK(taken.Vertices.at(1).Widths == Widths{.Start = 3, .End = 0});
    CHECK(taken.Vertices.at(3).Widths == Widths{});
  }

  TEST_CASE("lwpolyline: a bulge where an arc starts, an open last one's too")
  {
    const LwPolyline taken = Replayed<LwPolyline>(Traced("2D1"));
    REQUIRE(taken.Vertices.size()    == 3);
    CHECK(taken.Vertices.at(0).Bulge == 0);
    CHECK(taken.Vertices.at(1).Bulge < 0);
    CHECK(taken.Vertices.at(2).Bulge < 0);
  }

  TEST_CASE("lwpolyline: its elevation and thickness before its vertices")
  {
    CHECK(Replayed<LwPolyline>(Traced("2D3")).Elevation == 5);
    CHECK(Replayed<LwPolyline>(Traced("2D4")).Thickness == 3);
  }

  TEST_CASE("lwpolyline: on another plane, its extrusion after its vertices")
  {
    const LwPolyline taken = Replayed<LwPolyline>(Traced("2D5"));
    CHECK(taken.Extrusion.Y == -1);
  }

  TEST_CASE("lwpolyline: a vertex whole, widths, bulge, then its identifier")
  {
    const LwPolyline taken = Replayed<LwPolyline>(Traced("2E0"));
    REQUIRE(taken.Vertices.size()     == 3);
    CHECK(taken.Vertices.at(0).Widths == Widths{.Start = 1, .End = 2});
    CHECK(taken.Vertices.at(0).Bulge  == 0.5);
    CHECK(taken.Vertices.at(0).Id     == 7);
    CHECK(taken.Vertices.at(1).Id     == 0);
    CHECK(taken.Vertices.at(2).Id     == 9);
  }

  TEST_CASE("lwpolyline: identifiers from R2010, in an xrecord below")
  {
    CHECK(Replayed<LwPolyline>(Traced("2E2", Version::R2010), Version::R2010).Vertices.front().Id
          == 3);
    const std::string below = Traced("2E2", Version::R2007);
    CHECK_FALSE(below.contains(" 91\r\n"));
    CHECK(Replayed<LwPolyline>(below, Version::R2007).Vertices.front().Id == 0);
  }

  TEST_CASE("lwpolyline: a vertex whole comes back in every release from R2000")
  {
    for (const Version release : Releases)
    {
      CAPTURE(release);
      const LwPolyline taken = Replayed<LwPolyline>(Traced("2E0", release), release);
      CHECK(taken.Vertices.size()        == 3);
      CHECK(taken.Vertices.front().Bulge == 0.5);
    }
  }

  TEST_CASE("lwpolyline: a 43 kept beside vertex widths, AutoCAD's over theirs")
  {
    constexpr std::string_view pair = " 40\r\n0.5\r\n 41\r\n2.0\r\n";
    for (const std::string_view width : {" 43\r\n1.5\r\n", " 43\r\n0.0\r\n", ""})
    {
      CAPTURE(width);
      const std::string text =
          Joined({" 90\r\n        2\r\n 70\r\n     0\r\n", width, Origin, pair, Corner, pair});
      const LwPolyline taken = Replayed<LwPolyline>(text);
      CHECK(taken.Width.has_value()       == !width.empty());
      CHECK(taken.Vertices.front().Widths == Widths{.Start = 0.5, .End = 2});
    }
  }

  TEST_CASE("lwpolyline: no 43 and no widths stays so")
  {
    const std::string text = Joined({" 90\r\n        1\r\n 70\r\n     0\r\n", Origin});
    CHECK_FALSE(Replayed<LwPolyline>(text).Width);
  }

  TEST_CASE("lwpolyline: an end width alone, its start 0, as AutoCAD")
  {
    const LwPolyline taken = Read<LwPolyline>(
        Joined({" 90\r\n        1\r\n 70\r\n     0\r\n", Origin, " 41\r\n2.0\r\n"}));
    CHECK(taken.Vertices.front().Widths == Widths{.Start = 0, .End = 2});
    CHECK(Written(taken).contains(" 40\r\n0.0\r\n 41\r\n2.0\r\n"));
  }

  TEST_CASE("lwpolyline: a bulge and an identifier of 0 left out, as AutoCAD")
  {
    const LwPolyline taken =
        Read<LwPolyline>(Joined({" 90\r\n        1\r\n 70\r\n     0\r\n 43\r\n0.0\r\n", Origin,
                                 " 42\r\n0.0\r\n 91\r\n        0\r\n"}));
    const std::string text = Written(taken);
    CHECK_FALSE(text.contains(" 42\r\n"));
    CHECK_FALSE(text.contains(" 91\r\n"));
  }

  TEST_CASE("lwpolyline: an identifier before the widths, put after the bulge")
  {
    const LwPolyline taken =
        Read<LwPolyline>(Joined({" 90\r\n        1\r\n 70\r\n     0\r\n", Origin,
                                 " 91\r\n        7\r\n 40\r\n1.0\r\n 41\r\n2.0\r\n"}));
    CHECK(Written(taken).ends_with(" 40\r\n1.0\r\n 41\r\n2.0\r\n 91\r\n        7\r\n"));
  }

  TEST_CASE("lwpolyline: the 10s decide, whatever 90 says, and 90 is theirs")
  {
    const std::string counted = " 90\r\n        3\r\n 70\r\n     0\r\n";
    for (const std::string_view given :
         {" 90\r\n        2\r\n 70\r\n     0\r\n", " 90\r\n        5\r\n 70\r\n     0\r\n",
          " 90\r\n        0\r\n 70\r\n     0\r\n", " 70\r\n     0\r\n"})
    {
      CAPTURE(given);
      const LwPolyline taken =
          Read<LwPolyline>(Joined({given, " 43\r\n0.0\r\n", Origin, Corner, Top}));
      REQUIRE(taken.Vertices.size()          == 3);
      CHECK(taken.Vertices.back().Position.Y == 10);
      CHECK(taken.Record.Base.Extra.empty());
      CHECK(Written(taken).contains(counted));
    }
  }

  TEST_CASE("lwpolyline: none at all, kept as none")
  {
    const std::string text = Joined({" 90\r\n        0\r\n 70\r\n     0\r\n 43\r\n0.0\r\n"});
    CHECK(Replayed<LwPolyline>(text).Vertices.empty());
  }

  TEST_CASE("lwpolyline: groups out of their place, written back in it")
  {
    const LwPolyline taken =
        Read<LwPolyline>(Joined({" 43\r\n1.0\r\n 90\r\n        2\r\n 210\r\n0.0\r\n220\r\n0.0\r\n"
                                 "230\r\n-1.0\r\n",
                                 Origin, Corner, " 38\r\n2.0\r\n 39\r\n1.0\r\n 70\r\n     1\r\n"}));
    CHECK(taken.Width             == 1.0);
    CHECK(taken.Flags             == 1);
    CHECK(taken.Elevation         == 2);
    CHECK(taken.Thickness         == 1);
    CHECK(taken.Extrusion.Z       == -1);
    REQUIRE(taken.Vertices.size() == 2);
    CHECK(Written(taken)
          == Joined({" 90\r\n        2\r\n 70\r\n     1\r\n 43\r\n1.0\r\n"
                     " 38\r\n2.0\r\n 39\r\n1.0\r\n",
                     Origin, Corner, "210\r\n0.0\r\n220\r\n0.0\r\n230\r\n-1.0\r\n"}));
  }

  TEST_CASE("lwpolyline: a z kept after its y, the one place AutoCAD takes it")
  {
    constexpr std::string_view z     = " 30\r\n5.0\r\n";
    const LwPolyline           taken = Replayed<LwPolyline>(
        Joined({" 90\r\n        2\r\n 70\r\n     0\r\n", Origin, z, Corner, z}));
    CHECK(taken.Vertices.front().Position.Z == 5);
  }

  TEST_CASE("lwpolyline: a 91 of any release stays on its vertex")
  {
    const LwPolyline taken =
        Replayed<LwPolyline>(Joined({" 90\r\n        2\r\n 70\r\n     0\r\n", Origin,
                                     " 91\r\n        7\r\n", Corner, " 91\r\n        9\r\n"}),
                             Version::R2004);
    CHECK(taken.Vertices.front().Id == 7);
    CHECK(taken.Vertices.back().Id  == 9);
  }

  TEST_CASE("lwpolyline: its identifiers written from R2010, or in the release they came from")
  {
    const auto taken = Replayed<LwPolyline>(Traced("2E2", Version::R2010), Version::R2010);
    REQUIRE(taken.Vertices.front().Id == 3);
    for (const Version release : {Version::R2000, Version::R2004, Version::R2007})
    {
      CAPTURE(release);
      Fields::Sheet narrowed{release};
      narrowed.To.From = Version::R2010;
      REQUIRE(Given(narrowed.To, taken));
      CHECK_FALSE(narrowed.Out.contains(" 91\r\n"));
    }
  }

  TEST_CASE("lwpolyline: flags kept whole, as the file gave them")
  {
    const std::string text = Joined({" 90\r\n        0\r\n 70\r\n   513\r\n 43\r\n0.0\r\n"});
    CHECK(Replayed<LwPolyline>(text).Flags == 513);
  }

  TEST_CASE("lwpolyline: a new one, its count, flags and a width of 0")
  {
    LwPolyline fresh{};
    fresh.Record.Base.Self  = Handle{0x2a};
    fresh.Record.Base.Owner = Handle{0x1f};
    CHECK(Written(fresh) == Joined({" 90\r\n        0\r\n 70\r\n     0\r\n 43\r\n0.0\r\n"}));
  }
} // namespace DxfIO::Entities
