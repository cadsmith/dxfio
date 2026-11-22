#include "dxfio/drawing.hpp"

#include "dxfio/create.hpp"
#include "dxfio/edit/add.hpp"

#include <doctest/doctest.h>

#include <variant>

namespace DxfIO
{
  TEST_CASE("drawing: an entity is found by its handle, in the entities or in a block")
  {
    Drawing drawing = Create();
    REQUIRE(Edit::AddBlock(drawing, "Door"));
    const auto drawn = Edit::Add(drawing, Entities::Line{});
    const auto held  = Edit::Add(drawing, Entities::Circle{}, "Door");
    REQUIRE(drawn);
    REQUIRE(held);
    Entity *line = FindEntity(drawing, *drawn);
    REQUIRE(line != nullptr);
    CHECK(std::holds_alternative<Entities::Line>(*line));
    const Drawing &fixed  = drawing;
    const Entity  *circle = FindEntity(fixed, *held);
    REQUIRE(circle != nullptr);
    CHECK(std::holds_alternative<Entities::Circle>(*circle));
    CHECK(FindEntity(fixed, *drawn)  == line);
    CHECK(FindEntity(drawing, *held) == circle);
    CHECK(FindEntity(drawing, Handle{.Value = 0xfffff}) == nullptr);
    CHECK(FindEntity(fixed, Handle{}) == nullptr);
  }

  TEST_CASE("drawing: the null handle finds nothing, though records with no handle are there")
  {
    Drawing drawing = Create(Version::R12);
    drawing.Entities.emplace_back(Entities::Line{});
    drawing.Blocks.front().Entities.emplace_back(Entities::Circle{});
    drawing.Objects.emplace_back(Objects::Dictionary{});
    const Drawing &fixed = drawing;
    CHECK(FindEntity(drawing, Handle{}) == nullptr);
    CHECK(FindEntity(fixed, Handle{})   == nullptr);
    CHECK(FindObject(drawing, Handle{}) == nullptr);
    CHECK(FindObject(fixed, Handle{})   == nullptr);
  }

  TEST_CASE("drawing: an object is found by its handle")
  {
    Drawing      drawing = Create();
    const Handle root    = BaseOf(drawing.Objects.front()).Self;
    Object      *found   = FindObject(drawing, root);
    REQUIRE(found != nullptr);
    CHECK(TypeOf(*found) == "DICTIONARY");
    const Drawing &fixed = drawing;
    CHECK(FindObject(fixed, root) == found);
    CHECK(FindObject(drawing, Handle{.Value = 0xfffff}) == nullptr);
    CHECK(FindObject(fixed, Handle{.Value = 0xfffff})   == nullptr);
  }
} // namespace DxfIO
