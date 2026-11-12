#include "dxfio/create.hpp"

#include "header/io.hpp"
#include "releases.hpp"
#include "tags/codes.hpp"
#include "tags/tags.hpp"
#include "written.hpp"

#include "dxfio/load.hpp"
#include "dxfio/save.hpp"

#include <doctest/doctest.h>

#include <algorithm>
#include <array>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <vector>

namespace DxfIO
{
  namespace
  {
    struct Pointers
    {
      std::set<Handle>    Own;
      std::vector<Handle> Named;
      Handle              Seed{};
    };

    auto Gathered(std::string_view text) -> Pointers
    {
      const auto all = Tags::All(text);
      REQUIRE(all);
      Pointers out;
      bool     seeding  = false;
      bool     dimstyle = false;
      for (const Tags::Tag &one : *all)
      {
        dimstyle         = one.Code == 0 ? Tags::Text(one) == "DIMSTYLE" : dimstyle;
        const bool named = dimstyle && one.Code == 5;
        if (Tags::Codes::Of(one.Code) != Tags::Codes::Type::Handle || named)
        {
          seeding = one.Code == 9 && Tags::Text(one) == "$HANDSEED";
          continue;
        }
        const std::optional<Handle> found = Parsed(Tags::Text(one));
        REQUIRE(found);
        if (seeding)
        {
          out.Seed = *found;
        }
        else if (one.Code == 5 || one.Code == 105)
        {
          CHECK(out.Own.insert(*found).second);
        }
        else if (*found)
        {
          out.Named.push_back(*found);
        }
      }
      return out;
    }

    void Pointing(Version release)
    {
      CAPTURE(release);
      const auto text = Tests::Written(Create(release), Options{.Release = release});
      REQUIRE(text);
      const Pointers all = Gathered(*text);
      for (const Handle one : all.Named)
      {
        CAPTURE(one.Value);
        CHECK(all.Own.contains(one));
      }
      REQUIRE_FALSE(all.Own.empty());
      CHECK(all.Seed.Value == all.Own.rbegin()->Value + 1);
    }
  } // namespace

  TEST_CASE("created: in each release, read back whole and saved the same")
  {
    for (const Version release : Releases)
    {
      CAPTURE(release);
      const Document drawing = Create(release);
      CHECK(Header::Declared(drawing.Header) == release);
      const auto once = Tests::Written(drawing, Options{.Release = release});
      REQUIRE(once);
      const auto read = Load(*once);
      REQUIRE(read);
      CHECK(read->Issues.empty());
      const auto twice = Tests::Written(read->Document, Options{.Release = release});
      REQUIRE(twice);
      CHECK(*twice == *once);
    }
  }

  TEST_CASE("created: every handle its own, every pointer to a record, the seed "
            "past them")
  {
    Pointing(Version::R13);
    Pointing(Version::R14);
    for (const Version release : Releases)
    {
      Pointing(release);
    }
  }

  TEST_CASE("created: the releases before R2000 read back whole, saved the same")
  {
    for (const Version release : {Version::R10, Version::R12, Version::R13, Version::R14})
    {
      CAPTURE(release);
      const Document drawing = Create(release);
      CHECK(Header::Declared(drawing.Header) == release);
      const auto once = Tests::Written(drawing, Options{.Release = release});
      REQUIRE(once);
      const auto read = Load(*once);
      REQUIRE(read);
      CHECK(read->Issues.empty());
      const auto twice = Tests::Written(read->Document, Options{.Release = release});
      REQUIRE(twice);
      CHECK(*twice == *once);
    }
  }

  TEST_CASE("created: R13 and R14 hold only the objects those releases have")
  {
    for (const Version release : {Version::R13, Version::R14})
    {
      const Document drawing = Create(release);
      CHECK(drawing.Objects.size() == 4);
      CHECK(drawing.Classes.empty());
      CHECK(drawing.Dimstyle.Head.Base.Extra.empty());
      for (const Tables::BlockRecord &one : drawing.BlockRecord.Records)
      {
        CHECK_FALSE(one.Layout);
      }
      CHECK_FALSE(drawing.Layer.Records.front().Style);
      CHECK_FALSE(drawing.Layer.Records.front().Material);
    }
    const Document modern = Create(Version::R2000);
    CHECK_FALSE(modern.Dimstyle.Head.Base.Extra.empty());
    CHECK(modern.BlockRecord.Records.front().Layout);
    CHECK(modern.Layer.Records.front().Style);
  }

  TEST_CASE("created: R12 holds its own sections and its spaces' names")
  {
    const Document drawing = Create(Version::R12);
    CHECK(drawing.Order.size() == 4);
    CHECK(drawing.Objects.empty());
    CHECK(drawing.Blocks.front().Begin.Name == "$MODEL_SPACE");
  }
} // namespace DxfIO
