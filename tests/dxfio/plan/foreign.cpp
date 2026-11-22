#include "plan/foreign.hpp"

#include "objects/io.hpp"

#include "dxfio/create.hpp"
#include "dxfio/entities/line.hpp"

#include <doctest/doctest.h>

#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace DxfIO::Plan
{
  namespace
  {
    constexpr Handle Kept{0x10};

    constexpr Handle Gone{0x11};

    auto Drawn() -> Drawing
    {
      Drawing out{};
      Unknown style{};
      style.Type      = Symbol{"VISUALSTYLE"};
      style.Base.Self = Gone;
      Objects::Scale scale{};
      scale.Base.Self = Kept;
      Objects::Dictionary names{};
      names.Entries = {Objects::Entry{.Name = "Kept", .Soft = Kept, .Hard = {}},
                       Objects::Entry{.Name = "Gone", .Soft = Gone, .Hard = {}}};
      out.Objects   = {Object{names}, Object{scale}, Object{style}};
      return out;
    }
  } // namespace

  TEST_CASE("foreign: what no kind types stays behind, its entry with it, told")
  {
    Drawing            drawing = Drawn();
    std::vector<Issue> issues;
    Foreign(drawing, &issues);
    REQUIRE(drawing.Objects.size() == 2);
    const auto *names = std::get_if<Objects::Dictionary>(&drawing.Objects.front());
    REQUIRE(names != nullptr);
    REQUIRE(names->Entries.size()     == 1);
    CHECK(names->Entries.front().Name == "Kept");
    CHECK(issues
          == std::vector{Issue{.Code    = IssueCode::NotKept,
                               .Subject = Gone,
                               .What    = "VISUALSTYLE left out, its groups are in R12's form"}});
  }

  TEST_CASE("foreign: an entry held hard goes the same way")
  {
    Drawing drawing = Drawn();
    std::get<Objects::Dictionary>(drawing.Objects.front()).Entries.at(1) =
        Objects::Entry{.Name = "Gone", .Soft = {}, .Hard = Gone};
    Foreign(drawing);
    CHECK(std::get<Objects::Dictionary>(drawing.Objects.front()).Entries.size() == 1);
  }

  TEST_CASE("foreign: a default dictionary loses the entry and the default it held, and every "
            "reactor or extension dictionary naming it goes")
  {
    Drawing             drawing = Drawn();
    Objects::Dictionary styles{};
    styles.Entries = {Objects::Entry{.Name = "Gone", .Soft = Gone, .Hard = {}}};
    styles.Default = Gone;
    drawing.Objects.emplace_back(styles);
    Objects::Dictionary kept{};
    kept.Default = Kept;
    drawing.Objects.emplace_back(kept);
    Entities::Line line{};
    line.Record.Base.Reactors   = {Kept, Gone};
    line.Record.Base.Dictionary = Gone;
    drawing.Entities.emplace_back(line);
    Entities::Line other{};
    other.Record.Base.Dictionary = Kept;
    drawing.Entities.emplace_back(other);
    Foreign(drawing);
    const auto &held = std::get<Objects::Dictionary>(drawing.Objects.at(2));
    CHECK(held.Entries.empty());
    CHECK(held.Default == Handle{});
    CHECK(std::get<Objects::Dictionary>(drawing.Objects.at(3)).Default == Kept);
    const auto &first = std::get<Entities::Line>(drawing.Entities.front());
    CHECK(first.Record.Base.Reactors == std::vector{Kept});
    CHECK_FALSE(first.Record.Base.Dictionary);
    CHECK(std::get<Entities::Line>(drawing.Entities.back()).Record.Base.Dictionary == Kept);
  }

  TEST_CASE("foreign: what an object left out owns goes with it, and a header variable naming "
            "either is cleared")
  {
    Drawing             drawing = Drawn();
    Objects::Dictionary owned{};
    owned.Base.Self  = Handle{0x12};
    owned.Base.Owner = Gone;
    drawing.Objects.emplace_back(owned);
    Objects::XRecord deeper{};
    deeper.Base.Self  = Handle{0x13};
    deeper.Base.Owner = Handle{0x12};
    drawing.Objects.insert(drawing.Objects.begin(), Object{deeper});
    Objects::XRecord handless{};
    handless.Base.Owner = Gone;
    drawing.Objects.emplace_back(handless);
    Objects::XRecord pointing{};
    pointing.Base.Self = Handle{0x14};
    pointing.Data      = {Tag{.Code = 340, .Value = Handle{0x13}}, Tag{.Code = 340, .Value = Kept},
                          Tag{.Code = 1, .Value = std::string{"note"}}};
    drawing.Objects.emplace_back(pointing);
    drawing.Header.push_back(Header::Variable{.Name   = "$CMATERIAL",
                                              .Values = {Tag{.Code = 347, .Value = Handle{0x13}}}});
    drawing.Header.push_back(
        Header::Variable{.Name = "$CLAYER", .Values = {Tag{.Code = 8, .Value = std::string{"0"}}}});
    std::vector<Issue> issues;
    Foreign(drawing, &issues);
    REQUIRE(drawing.Objects.size() == 3);
    CHECK(std::get<Objects::XRecord>(drawing.Objects.back()).Data
          == std::vector{Tag{.Code = 340, .Value = Handle{}}, Tag{.Code = 340, .Value = Kept},
                         Tag{.Code = 1, .Value = std::string{"note"}}});
    CHECK_FALSE(std::get<Handle>(drawing.Header.front().Values.front().Value));
    CHECK(drawing.Header.back().Values.front().Value == Content{std::string{"0"}});
    REQUIRE(issues.size()     == 4);
    CHECK(issues.front().What == "XRECORD left out with the object that owns it");
    CHECK(issues.at(2).What   == "DICTIONARY left out with the object that owns it");
    CHECK(issues.back().What  == "XRECORD left out with the object that owns it");
  }

  TEST_CASE("foreign: an unknown object with no handle takes no entry with it")
  {
    Drawing drawing = Drawn();
    drawing.Objects.emplace_back(Unknown{.Type = Symbol{"FOO"}, .Base = {}, .Tags = {}});
    Foreign(drawing);
    REQUIRE(drawing.Objects.size() == 2);
    const auto &names = std::get<Objects::Dictionary>(drawing.Objects.front());
    REQUIRE(names.Entries.size()     == 1);
    CHECK(names.Entries.front().Name == "Kept");
  }

  TEST_CASE("foreign: a drawing with nothing foreign comes back the same")
  {
    Drawing drawing = Drawn();
    drawing.Objects.pop_back();
    std::get<Objects::Dictionary>(drawing.Objects.front()).Entries.pop_back();
    const Drawing      before = drawing;
    std::vector<Issue> issues;
    Foreign(drawing, &issues);
    CHECK(drawing == before);
    CHECK(issues.empty());
  }
} // namespace DxfIO::Plan
