#include "dxfio/edit/find.hpp"

#include "dxfio/create.hpp"
#include "dxfio/edit/add.hpp"

#include <doctest/doctest.h>

#include <string>
#include <variant>

namespace DxfIO::Edit
{
TEST_CASE("edit: names compare without case")
{
  CHECK(Same("Walls", "WALLS"));
  CHECK(Same("", ""));
  CHECK_FALSE(Same("Walls", "Wall"));
  CHECK_FALSE(Same("Walls", "Halls"));
}

TEST_CASE("edit: an entity is found by its handle, in the entities or in a block")
{
  Document drawing = Create();
  REQUIRE(AddBlock(drawing, "Door"));
  const auto drawn = Add(drawing, Entities::Line{});
  const auto held = Add(drawing, Entities::Circle{}, "Door");
  REQUIRE(drawn);
  REQUIRE(held);
  Entity *line = FindEntity(drawing, *drawn);
  REQUIRE(line != nullptr);
  CHECK(std::holds_alternative<Entities::Line>(*line));
  const Document &fixed = drawing;
  const Entity *circle = FindEntity(fixed, *held);
  REQUIRE(circle != nullptr);
  CHECK(std::holds_alternative<Entities::Circle>(*circle));
  CHECK(FindEntity(fixed, *drawn) == line);
  CHECK(FindEntity(drawing, *held) == circle);
  CHECK(FindEntity(drawing, Handle{.Value = 0xfffff}) == nullptr);
  CHECK(FindEntity(fixed, Handle{}) == nullptr);
}

TEST_CASE("edit: the null handle finds nothing, though records with no handle are there")
{
  Document drawing = Create(Version::R12);
  drawing.Entities.emplace_back(Entities::Line{});
  drawing.Blocks.front().Entities.emplace_back(Entities::Circle{});
  drawing.Objects.emplace_back(Objects::Dictionary{});
  const Document &fixed = drawing;
  CHECK(FindEntity(drawing, Handle{}) == nullptr);
  CHECK(FindEntity(fixed, Handle{}) == nullptr);
  CHECK(FindObject(drawing, Handle{}) == nullptr);
  CHECK(FindObject(fixed, Handle{}) == nullptr);
}

TEST_CASE("edit: an object is found by its handle")
{
  Document drawing = Create();
  const Handle root = BaseOf(drawing.Objects.front()).Self;
  Object *found = FindObject(drawing, root);
  REQUIRE(found != nullptr);
  CHECK(TypeOf(*found) == "DICTIONARY");
  const Document &fixed = drawing;
  CHECK(FindObject(fixed, root) == found);
  CHECK(FindObject(drawing, Handle{.Value = 0xfffff}) == nullptr);
  CHECK(FindObject(fixed, Handle{.Value = 0xfffff}) == nullptr);
}

TEST_CASE("edit: a block and a table record are found by name, without case")
{
  Document drawing = Create();
  Blocks::Block *model = FindBlock(drawing, "*MODEL_SPACE");
  REQUIRE(model != nullptr);
  CHECK(model->Begin.Name == "*Model_Space");
  const Document &fixed = drawing;
  CHECK(FindBlock(fixed, "*paper_space") != nullptr);
  CHECK(FindBlock(drawing, "Door") == nullptr);
  CHECK(FindBlock(fixed, "Door") == nullptr);

  Tables::Ltype *continuous = Find(drawing.Ltype, "CONTINUOUS");
  REQUIRE(continuous != nullptr);
  CHECK(continuous->Record.Name == "Continuous");
  CHECK(Find(fixed.Layer, "0") == &drawing.Layer.Records.front());
  CHECK(Find(drawing.Layer, "Walls") == nullptr);
  CHECK(Find(fixed.Style, "Bold") == nullptr);
}
} // namespace DxfIO::Edit
