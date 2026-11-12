#include "header/created.hpp"

#include "../disk.hpp"
#include "header/io.hpp"
#include "unknown/io.hpp"

#include "dxfio/load.hpp"

#include <doctest/doctest.h>

#include <algorithm>
#include <array>
#include <string>
#include <string_view>
#include <vector>

namespace DxfIO::Header
{
  namespace
  {
    struct Drawing
    {
      Version          Release{};
      std::string_view File;
    };

    constexpr std::array Drawings{
        Drawing{.Release = Version::R12  , .File = "empty-r12.dxf"  },
        Drawing{.Release = Version::R2000, .File = "empty-r2000.dxf"},
        Drawing{.Release = Version::R2004, .File = "empty-r2004.dxf"},
        Drawing{.Release = Version::R2007, .File = "empty-r2007.dxf"},
        Drawing{.Release = Version::R2010, .File = "empty-r2010.dxf"},
        Drawing{.Release = Version::R2013, .File = "empty-r2013.dxf"},
        Drawing{.Release = Version::R2018, .File = "empty-r2018.dxf"},
    };

    constexpr std::array<std::string_view, 10> Unset{
        "$LASTSAVEDBY", "$TDCREATE",   "$TDUCREATE", "$TDUPDATE",        "$TDUUPDATE",
        "$TDINDWG",     "$TDUSRTIMER", "$HANDSEED",  "$FINGERPRINTGUID", "$VERSIONGUID",
    };

    auto Autocad(const Drawing &one) -> std::vector<Variable>
    {
      const auto read = Load(Acad(one.File));
      REQUIRE(read);
      return read->Document.Header;
    }

    auto Kept(const Variable &one, Version release) -> bool
    {
      const bool upper =
          release == Version::R12 && (one.Name == "$TEXTSTYLE" || one.Name == "$CELTYPE");
      return !upper && std::ranges::find(Unset, one.Name) == Unset.end();
    }

    auto Compared(std::vector<Variable> all, Version release) -> std::vector<Variable>
    {
      std::erase_if(all, [release](const Variable &one) -> bool { return !Kept(one, release); });
      return all;
    }
  } // namespace

  TEST_CASE("created: AutoCAD's own header in every release")
  {
    for (const Drawing &one : Drawings)
    {
      CAPTURE(one.File);
      const std::vector<Variable> made  = Created(one.Release);
      const std::vector<Variable> given = Autocad(one);
      REQUIRE(made.size() == given.size());
      for (std::size_t at = 0; at < made.size(); ++at)
      {
        CHECK(made.at(at).Name == given.at(at).Name);
      }
      CHECK(Compared(made, one.Release) == Compared(given, one.Release));
    }
  }

  TEST_CASE("created: a new drawing declares the release asked")
  {
    CHECK(Declared(Created())             == Version::R2018);
    CHECK(Declared(Created(Version::R14)) == Version::R14);
    CHECK(Declared(Created(Version::R12)) == Version::R12);
  }
} // namespace DxfIO::Header
