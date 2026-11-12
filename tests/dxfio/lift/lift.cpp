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
    auto Raised(const Document &drawing, Version release) -> Loaded
    {
      const auto saved = Tests::Written(drawing, {.Release = release});
      REQUIRE(saved);
      const auto back = Load(*saved);
      REQUIRE(back);
      return *back;
    }

    auto Owner(const Document &drawing, std::string_view block) -> Handle
    {
      const Tables::BlockRecord *found = Edit::Find(drawing.BlockRecord, block);
      REQUIRE(found != nullptr);
      return found->Record.Base.Self;
    }

    auto Drawn(Version release) -> Document
    {
      Document out = Create(release);
      REQUIRE(Edit::Add(out, Entities::Line{}));
      return out;
    }
  } // namespace

  TEST_CASE(
      "lift: an R12 drawing saved from R13 on is given what the release needs, and audits clean")
  {
    for (const Version release : {Version::R2000, Version::R2004, Version::R2018})
    {
      CAPTURE(Name(release));
      const Loaded back = Raised(Drawn(Version::R12), release);
      CHECK(back.Issues.empty());
      CHECK(Header::Declared(back.Document.Header) == release);
      CHECK(back.Document.Order
            == std::vector<std::string>{"HEADER", "CLASSES", "TABLES", "BLOCKS", "ENTITIES",
                                        "OBJECTS"});
      CHECK(Edit::Find(back.Document.BlockRecord, "*Model_Space") != nullptr);
      CHECK(Edit::Find(back.Document.BlockRecord, "*Paper_Space") != nullptr);
      CHECK(Edit::Find(back.Document.Ltype, "ByLayer") != nullptr);
      CHECK(Edit::Find(back.Document.Ltype, "ByBlock") != nullptr);
      CHECK_FALSE(back.Document.Objects.empty());
      CHECK_FALSE(back.Document.Classes.empty());
      CHECK(back.Document.Layer.Head.Base.Self);
      CHECK(Audit::Check(back.Document).Findings.empty());
      REQUIRE(back.Document.Entities.size() == 1);
      CHECK(BaseOf(back.Document.Entities.front()).Owner == Owner(back.Document, "*Model_Space"));
    }
  }

  TEST_CASE("lift: the same from R10, and nothing changes from R13 on")
  {
    const Loaded back = Raised(Drawn(Version::R10), Version::R2000);
    CHECK(Audit::Check(back.Document).Findings.empty());
    const Document kept = Lifted(Drawn(Version::R2000), Version::R2018);
    CHECK(kept == Drawn(Version::R2000));
  }

  TEST_CASE("lift: entities keep their handles, the rest take those the drawing leaves free")
  {
    Document       drawing = Create(Version::R12);
    Entities::Line line{};
    line.Record.Base.Self = Handle{1};
    REQUIRE(Edit::Add(drawing, std::move(line)));
    const Loaded back = Raised(drawing, Version::R2000);
    REQUIRE(back.Document.Entities.size()             == 1);
    CHECK(BaseOf(back.Document.Entities.front()).Self == Handle{1});
    CHECK(Audit::Check(back.Document).Findings.empty());
  }

  TEST_CASE("lift: a record of the drawing replaces the release's own by name, its values kept, "
            "its links given")
  {
    Document       drawing = Create(Version::R12);
    Tables::Layer *zero    = Edit::Find(drawing.Layer, "0");
    REQUIRE(zero != nullptr);
    zero->Color = 3;
    Tables::Layer walls{};
    walls.Record.Name = "WALLS";
    walls.Color       = 5;
    REQUIRE(Edit::Add(drawing, walls));
    const Loaded         back  = Raised(drawing, Version::R2000);
    const Tables::Layer *first = Edit::Find(back.Document.Layer, "0");
    REQUIRE(first != nullptr);
    CHECK(first->Color == 3);
    CHECK(first->Style);
    CHECK(first->Record.Base.Self);
    CHECK(first->Record.Base.Owner == back.Document.Layer.Head.Base.Self);
    const Tables::Layer *made = Edit::Find(back.Document.Layer, "WALLS");
    REQUIRE(made != nullptr);
    CHECK(made->Color             == 5);
    CHECK(made->Style             == first->Style);
    CHECK(made->Record.Base.Owner == back.Document.Layer.Head.Base.Self);
    CHECK(std::ranges::count(back.Document.Layer.Records, "0",
                             [](const Tables::Layer &one) -> std::string_view
                             { return one.Record.Name; })
          == 1);
    CHECK(Audit::Check(back.Document).Findings.empty());
  }

  TEST_CASE("lift: a block is given its record, its entities their owner")
  {
    Document drawing = Create(Version::R12);
    REQUIRE(Edit::AddBlock(drawing, "PART"));
    REQUIRE(Edit::Add(drawing, Entities::Circle{}, "PART"));
    const Loaded         back   = Raised(drawing, Version::R2000);
    const Handle         record = Owner(back.Document, "PART");
    const Blocks::Block *block  = Edit::FindBlock(back.Document, "PART");
    REQUIRE(block != nullptr);
    CHECK(block->Begin.Record.Base.Owner == record);
    CHECK(block->End.Record.Base.Owner   == record);
    CHECK(block->Begin.Record.Base.Self);
    REQUIRE(block->Entities.size() == 1);
    CHECK(BaseOf(block->Entities.front()).Owner == record);
    CHECK(Audit::Check(back.Document).Findings.empty());
  }

  TEST_CASE("lift: the R12 space blocks become the spaces, what they hold inside them")
  {
    Document drawing = Create(Version::R12);
    REQUIRE(Edit::FindBlock(drawing, "$PAPER_SPACE") != nullptr);
    REQUIRE(Edit::Add(drawing, Entities::Circle{}, "$PAPER_SPACE"));
    REQUIRE(Edit::Add(drawing, Entities::Arc{}, "$MODEL_SPACE"));
    REQUIRE(Edit::Add(drawing, Entities::Line{}, Edit::Space::Paper));
    const Loaded back = Raised(drawing, Version::R2000);
    CHECK(Edit::FindBlock(back.Document, "$PAPER_SPACE")        == nullptr);
    CHECK(Edit::Find(back.Document.BlockRecord, "$PAPER_SPACE") == nullptr);
    CHECK(Edit::FindBlock(back.Document, "$MODEL_SPACE")        == nullptr);
    const Blocks::Block *model = Edit::FindBlock(back.Document, "*Model_Space");
    REQUIRE(model != nullptr);
    REQUIRE(model->Entities.size() == 1);
    CHECK(BaseOf(model->Entities.front()).Owner == Owner(back.Document, "*Model_Space"));
    CHECK_FALSE(Entities::Paper(model->Entities.front()));
    const Handle         paper = Owner(back.Document, "*Paper_Space");
    const Blocks::Block *sheet = Edit::FindBlock(back.Document, "*Paper_Space");
    REQUIRE(sheet != nullptr);
    REQUIRE(sheet->Entities.size() == 1);
    CHECK(BaseOf(sheet->Entities.front()).Owner == paper);
    REQUIRE(back.Document.Entities.size()       == 1);
    CHECK(BaseOf(back.Document.Entities.front()).Owner == paper);
    CHECK(Entities::Paper(back.Document.Entities.front()));
    CHECK(Audit::Check(back.Document).Findings.empty());
  }
} // namespace DxfIO::Lift
