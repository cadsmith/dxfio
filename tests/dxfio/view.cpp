#include "dxfio/view.hpp"

#include "dxfio/create.hpp"

#include <doctest/doctest.h>

#include <algorithm>
#include <variant>

namespace DxfIO::View
{
  TEST_CASE("view: names compare without case")
  {
    CHECK(Matches("Walls", "WALLS"));
    CHECK(Matches("", ""));
    CHECK_FALSE(Matches("Walls", "Wall"));
    CHECK_FALSE(Matches("Walls", "Halls"));
  }

  TEST_CASE("view: a block and a table record are found by name, without case")
  {
    Drawing             drawing = Create();
    Blocks::Definition *model   = Block(drawing, "*MODEL_SPACE");
    REQUIRE(model != nullptr);
    CHECK(model->Block.Name == "*Model_Space");
    const Drawing &fixed = drawing;
    CHECK(Block(fixed, "*paper_space") != nullptr);
    CHECK(Block(drawing, "Door") == nullptr);
    CHECK(Block(fixed, "Door")   == nullptr);

    Tables::LType *continuous = Record(drawing.LType, "CONTINUOUS");
    REQUIRE(continuous != nullptr);
    CHECK(continuous->Record.Name        == "Continuous");
    CHECK(Record(fixed.Layer, "0")       == &drawing.Layer.Records.front());
    CHECK(Record(drawing.Layer, "Walls") == nullptr);
    CHECK(Record(fixed.Style, "Bold")    == nullptr);
  }

  TEST_CASE("view: a table record is found by its kind, in the table of that kind")
  {
    Drawing        drawing = Create();
    const Drawing &fixed   = drawing;
    CHECK(Record<Tables::LType>(drawing, "continuous") == Record(drawing.LType, "Continuous"));
    CHECK(Record<Tables::Layer>(fixed, "0")            == &drawing.Layer.Records.front());
    CHECK(Record<Tables::Style>(fixed, "standard") != nullptr);
    CHECK(Record<Tables::AppId>(fixed, "ACAD") != nullptr);
    CHECK(Record<Tables::DimStyle>(fixed, "Bold") == nullptr);
    CHECK(Record<Tables::BlockRecord>(fixed, "*model_space") != nullptr);
    CHECK(&TableOf<Tables::View>(fixed)    == &drawing.View);
    CHECK(&TableOf<Tables::Ucs>(drawing)   == &drawing.Ucs);
    CHECK(&TableOf<Tables::VPort>(drawing) == &drawing.VPort);
  }

  TEST_CASE("view: model and paper space are their block records, none before R13")
  {
    const Drawing              drawing = Create();
    const Tables::BlockRecord *model   = ModelSpace(drawing);
    REQUIRE(model != nullptr);
    CHECK(model->Record.Name == "*Model_Space");
    const Tables::BlockRecord *paper = PaperSpace(drawing);
    REQUIRE(paper != nullptr);
    CHECK(paper->Record.Name == "*Paper_Space");

    const Drawing old = Create(Version::R12);
    CHECK(ModelSpace(old) == nullptr);
    CHECK(PaperSpace(old) == nullptr);
  }

  TEST_CASE("view: the layouts in tab order, Model first, whatever the order of the objects")
  {
    Drawing drawing = Create();
    std::ranges::reverse(drawing.Objects);
    const auto layouts = Layouts(drawing);
    REQUIRE(layouts.size() >= 2);
    CHECK(layouts.front()->Name == "Model");
    CHECK(std::ranges::is_sorted(layouts, {},
                                 [](const Objects::Layout *one) -> int { return one->Order; }));
    CHECK(Layout(drawing, "MODEL")   == layouts.front());
    CHECK(Layout(drawing, "Nowhere") == nullptr);
    CHECK(Layouts(Create(Version::R12)).empty());
  }

  TEST_CASE("view: the root dictionary, and the objects its entries lead to")
  {
    const Drawing              drawing = Create();
    const Objects::Dictionary *root    = Root(drawing);
    REQUIRE(root != nullptr);
    CHECK(root == std::get_if<Objects::Dictionary>(&drawing.Objects.front()));
    const Objects::Dictionary *layouts = Dictionary(drawing, "acad_layout");
    REQUIRE(layouts != nullptr);
    const Object *model = Entry(drawing, *layouts, "model");
    REQUIRE(model != nullptr);
    CHECK(std::holds_alternative<Objects::Layout>(*model));
    CHECK(Entry(drawing, *root, "NOPE") == nullptr);
    CHECK(Dictionary(drawing, "NOPE")   == nullptr);
    CHECK(Root(Drawing{}) == nullptr);
    CHECK(Dictionary(Drawing{}, "ACAD_LAYOUT") == nullptr);
  }

  TEST_CASE("view: the root is the dictionary no one owns, wherever it stands")
  {
    Drawing drawing = Create();
    std::ranges::reverse(drawing.Objects);
    const Objects::Dictionary *root = Root(drawing);
    REQUIRE(root != nullptr);
    CHECK_FALSE(root->Base.Owner);
    CHECK(root == std::get_if<Objects::Dictionary>(&drawing.Objects.back()));
  }

  TEST_CASE("view: an entry leads to its object whether the dictionary owns it hard or soft")
  {
    Drawing             drawing = Create();
    const Handle        root    = BaseOf(drawing.Objects.front()).Self;
    Objects::Dictionary both{};
    both.Entries = {
        Objects::Entry{.Name = "Hard", .Hard = root},
        Objects::Entry{.Name = "Soft", .Soft = root},
        Objects::Entry{.Name = "None"},
    };
    CHECK(Entry(drawing, both, "hard") == &drawing.Objects.front());
    CHECK(Entry(drawing, both, "soft") == &drawing.Objects.front());
    CHECK(Entry(drawing, both, "none") == nullptr);
  }
} // namespace DxfIO::View
