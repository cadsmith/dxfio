#include "empty/objects.hpp"

#include "../autocad.hpp"
#include "../disk.hpp"
#include "../fields/fixture.hpp"
#include "../releases.hpp"
#include "empty/roots.hpp"
#include "objects/io.hpp"

#include "dxfio/version.hpp"

#include <doctest/doctest.h>

#include <array>
#include <cstddef>
#include <format>
#include <set>
#include <span>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace DxfIO::Empty
{
  namespace
  {
    constexpr Handle Active{.Value = 0x99};

    struct Made
    {
      Seed                Handles;
      Roots               Ids{Taken(Handles)};
      std::vector<Object> All{Objects(Ids, Active, Handles)};
    };

    auto Text(const Object &one, Version release) -> std::string
    {
      Fields::Sheet page{release};
      REQUIRE(Objects::Write(page.To, std::span{&one, 1}));
      return page.Out;
    }

    auto Names(const Object &one) -> std::vector<std::string>
    {
      std::vector<std::string> out;
      for (const Objects::Entry &entry : std::get<Objects::Dictionary>(one).Entries)
      {
        out.push_back(entry.Name);
      }
      return out;
    }

    auto Theirs() -> std::vector<std::pair<std::size_t, std::string>>
    {
      std::vector<std::pair<std::size_t, std::string>> out{
          {1, "D"},   {3, "6A"},  {5, "17"},   {6, "19"},  {7, "E"},   {8, "10C"},
          {9, "7E"},  {11, "22"}, {12, "ED"},  {13, "EC"}, {14, "EE"}, {15, "12E"},
          {16, "18"}, {17, "F"},  {18, "10D"}, {35, "7F"}};
      for (std::size_t at = 1; at < 17; ++at)
      {
        out.emplace_back(18 + at, std::format("{:X}", 0x1BD + at));
      }
      return out;
    }
  } // namespace

  TEST_CASE("objects: the root names the dictionaries any reader looks up")
  {
    const Made fresh;
    REQUIRE(fresh.All.size() == 36);
    CHECK(Names(fresh.All.front())
          == std::vector<std::string>{"ACAD_GROUP", "ACAD_LAYOUT", "ACAD_MATERIAL",
                                      "ACAD_MLEADERSTYLE", "ACAD_MLINESTYLE", "ACAD_PLOTSETTINGS",
                                      "ACAD_PLOTSTYLENAME", "ACAD_SCALELIST", "ACAD_TABLESTYLE"});
    CHECK(Names(fresh.All.at(2)) == std::vector<std::string>{"Layout1", "Model"});
    CHECK(Names(fresh.All.at(4)) == std::vector<std::string>{"Standard"});
    const auto &styles = std::get<Objects::DictionaryWithDefault>(fresh.All.at(7));
    CHECK(styles.Fallback == fresh.Ids.Normal);
  }

  TEST_CASE("objects: each one owned and watched by the dictionary naming it")
  {
    const Made       fresh;
    std::set<Handle> seen;
    for (const Object &one : fresh.All)
    {
      std::visit(
          [&seen](const auto &kind) -> auto
          {
            if constexpr (requires { kind.Base; })
            {
              CHECK(seen.insert(kind.Base.Self).second);
            }
          },
          one);
    }
    CHECK(seen.size() == fresh.All.size() - 1);
    const auto &layout = std::get<Objects::Layout>(fresh.All.at(11));
    CHECK(layout.Base.Owner    == std::get<Objects::Dictionary>(fresh.All.at(2)).Base.Self);
    CHECK(layout.Base.Reactors == std::vector{layout.Base.Owner});
  }

  TEST_CASE("objects: written as AutoCAD writes its empty drawing's, from R2000")
  {
    const Made fresh;
    for (const Version release : Releases)
    {
      const std::string source = Acad(Fixture("empty", release));
      for (const auto &[at, self] : Theirs())
      {
        CAPTURE(release);
        CAPTURE(self);
        CHECK(Masked(Text(fresh.All.at(at), release)) == Masked(Autocad(source, self)));
      }
      std::string       paper   = Autocad(source, "D3");
      const std::string printer = "C:\\Documents and Settings\\basas\\Application Data\\Autodesk\\"
                                  "AutoCAD 2005\\R16.1\\enu\\plotters\\Default Windows System "
                                  "Printer.pc3";
      const std::size_t at      = paper.find(printer);
      REQUIRE(at != std::string::npos);
      paper.replace(at, printer.size(), "none_device");
      CAPTURE(release);
      CHECK(Masked(Text(fresh.All.at(10), release)) == Masked(paper));
    }
  }
} // namespace DxfIO::Empty
