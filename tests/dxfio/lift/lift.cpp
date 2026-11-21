#include "lift/lift.hpp"

#include "../disk.hpp"
#include "../written.hpp"
#include "entities/io.hpp"
#include "header/io.hpp"

#include "dxfio/audit/audit.hpp"
#include "dxfio/create.hpp"
#include "dxfio/edit/add.hpp"
#include "dxfio/edit/find.hpp"
#include "dxfio/load.hpp"

#include <doctest/doctest.h>

#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace DxfIO::Lift
{
  namespace
  {
    /**
     *  Saves a drawing to a release and reads the file back.
     */
    auto Raised(const Drawing &drawing, Version release) -> Loaded
    {
      const auto saved = Tests::Written(drawing, {.Release = release});
      REQUIRE(saved);
      const auto back = Load(*saved);
      REQUIRE(back);
      return *back;
    }

    auto Owner(const Drawing &drawing, std::string_view block) -> Handle
    {
      const Tables::BlockRecord *found = Edit::Find(drawing.BlockRecord, block);
      REQUIRE(found != nullptr);
      return found->Record.Base.Self;
    }

    auto Drawn(Version release) -> Drawing
    {
      Drawing out = Create(release);
      REQUIRE(Edit::Add(out, Entities::Line{}));
      return out;
    }
  } // namespace

  TEST_CASE(
      "lift: an R12 drawing saved from R13 on is given what the release needs, and audits clean")
  {
    for (const Version release : {Version::R2000, Version::R2004, Version::R2018})
    {
      CAPTURE(NameOf(release));
      const Loaded back = Raised(Drawn(Version::R12), release);
      CHECK(back.Issues.empty());
      CHECK(Header::Declared(back.Drawing.Header) == release);
      CHECK(back.Drawing.Order
            == std::vector<std::string>{"HEADER", "CLASSES", "TABLES", "BLOCKS", "ENTITIES",
                                        "OBJECTS"});
      CHECK(Edit::Find(back.Drawing.BlockRecord, "*Model_Space") != nullptr);
      CHECK(Edit::Find(back.Drawing.BlockRecord, "*Paper_Space") != nullptr);
      CHECK(Edit::Find(back.Drawing.LType, "ByLayer") != nullptr);
      CHECK(Edit::Find(back.Drawing.LType, "ByBlock") != nullptr);
      CHECK_FALSE(back.Drawing.Objects.empty());
      CHECK_FALSE(back.Drawing.Classes.empty());
      CHECK(back.Drawing.Layer.Head.Base.Self);
      CHECK(Audit::Check(back.Drawing).Findings.empty());
      REQUIRE(back.Drawing.Entities.size() == 1);
      CHECK(BaseOf(back.Drawing.Entities.front()).Owner == Owner(back.Drawing, "*Model_Space"));
    }
  }

  TEST_CASE("lift: the same from R10, and nothing changes from R13 on")
  {
    const Loaded back = Raised(Drawn(Version::R10), Version::R2000);
    CHECK(Audit::Check(back.Drawing).Findings.empty());
    const Drawing kept = Lifted(Drawn(Version::R2000), Version::R2018);
    CHECK(kept == Drawn(Version::R2000));
  }

  TEST_CASE("lift: entities keep their handles, the rest take those the drawing leaves free")
  {
    Drawing        drawing = Create(Version::R12);
    Entities::Line line{};
    line.Record.Base.Self = Handle{1};
    REQUIRE(Edit::Add(drawing, std::move(line)));
    const Loaded back = Raised(drawing, Version::R2000);
    REQUIRE(back.Drawing.Entities.size()             == 1);
    CHECK(BaseOf(back.Drawing.Entities.front()).Self == Handle{1});
    CHECK(Audit::Check(back.Drawing).Findings.empty());
  }

  TEST_CASE("lift: a record of the drawing replaces the release's own by name, its values kept, "
            "its links given")
  {
    Drawing        drawing = Create(Version::R12);
    Tables::Layer *zero    = Edit::Find(drawing.Layer, "0");
    REQUIRE(zero != nullptr);
    zero->Color = 3;
    Tables::Layer walls{};
    walls.Record.Name = "WALLS";
    walls.Color       = 5;
    REQUIRE(Edit::Add(drawing, walls));
    const Loaded         back  = Raised(drawing, Version::R2000);
    const Tables::Layer *first = Edit::Find(back.Drawing.Layer, "0");
    REQUIRE(first != nullptr);
    CHECK(first->Color == 3);
    CHECK(first->Style);
    CHECK(first->Record.Base.Self);
    CHECK(first->Record.Base.Owner == back.Drawing.Layer.Head.Base.Self);
    const Tables::Layer *made = Edit::Find(back.Drawing.Layer, "WALLS");
    REQUIRE(made != nullptr);
    CHECK(made->Color             == 5);
    CHECK(made->Style             == first->Style);
    CHECK(made->Record.Base.Owner == back.Drawing.Layer.Head.Base.Self);
    CHECK(std::ranges::count(back.Drawing.Layer.Records, "0",
                             [](const Tables::Layer &one) -> std::string_view
                             { return one.Record.Name; })
          == 1);
    CHECK(Audit::Check(back.Drawing).Findings.empty());
  }

  TEST_CASE("lift: a block is given its record, its entities their owner")
  {
    Drawing drawing = Create(Version::R12);
    REQUIRE(Edit::AddBlock(drawing, "PART"));
    REQUIRE(Edit::Add(drawing, Entities::Circle{}, "PART"));
    const Loaded              back   = Raised(drawing, Version::R2000);
    const Handle              record = Owner(back.Drawing, "PART");
    const Blocks::Definition *block  = Edit::FindBlock(back.Drawing, "PART");
    REQUIRE(block != nullptr);
    CHECK(block->Block.Record.Base.Owner  == record);
    CHECK(block->EndBlk.Record.Base.Owner == record);
    CHECK(block->Block.Record.Base.Self);
    REQUIRE(block->Entities.size() == 1);
    CHECK(BaseOf(block->Entities.front()).Owner == record);
    CHECK(Audit::Check(back.Drawing).Findings.empty());
  }

  TEST_CASE("lift: the R12 space blocks become the spaces, what they hold inside them")
  {
    Drawing drawing = Create(Version::R12);
    REQUIRE(Edit::FindBlock(drawing, "$PAPER_SPACE") != nullptr);
    REQUIRE(Edit::Add(drawing, Entities::Circle{}, "$PAPER_SPACE"));
    REQUIRE(Edit::Add(drawing, Entities::Arc{}, "$MODEL_SPACE"));
    REQUIRE(Edit::Add(drawing, Entities::Line{}, Edit::Space::Paper));
    const Loaded back = Raised(drawing, Version::R2000);
    CHECK(Edit::FindBlock(back.Drawing, "$PAPER_SPACE")        == nullptr);
    CHECK(Edit::Find(back.Drawing.BlockRecord, "$PAPER_SPACE") == nullptr);
    CHECK(Edit::FindBlock(back.Drawing, "$MODEL_SPACE")        == nullptr);
    const Blocks::Definition *model = Edit::FindBlock(back.Drawing, "*Model_Space");
    REQUIRE(model != nullptr);
    REQUIRE(model->Entities.size() == 1);
    CHECK(BaseOf(model->Entities.front()).Owner == Owner(back.Drawing, "*Model_Space"));
    CHECK_FALSE(Entities::Paper(model->Entities.front()));
    const Handle              paper = Owner(back.Drawing, "*Paper_Space");
    const Blocks::Definition *sheet = Edit::FindBlock(back.Drawing, "*Paper_Space");
    REQUIRE(sheet != nullptr);
    REQUIRE(sheet->Entities.size() == 1);
    CHECK(BaseOf(sheet->Entities.front()).Owner == paper);
    REQUIRE(back.Drawing.Entities.size()        == 1);
    CHECK(BaseOf(back.Drawing.Entities.front()).Owner == paper);
    CHECK(Entities::Paper(back.Drawing.Entities.front()));
    CHECK(Audit::Check(back.Drawing).Findings.empty());
  }
} // namespace DxfIO::Lift
