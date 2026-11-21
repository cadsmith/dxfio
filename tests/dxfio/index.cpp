#include "dxfio/index.hpp"

#include "disk.hpp"

#include "dxfio/load.hpp"

#include <doctest/doctest.h>

namespace DxfIO
{
  TEST_CASE("index: finds every entity and object as FindEntity and FindObject do")
  {
    for (const char *name : {"entities-r2000.dxf", "entities-r2018.dxf"})
    {
      CAPTURE(name);
      const auto loaded = Load(Acad(name));
      REQUIRE(loaded);
      const Drawing &drawing = loaded->Drawing;
      const Index    index{drawing};
      CHECK(&index.Drawn() == &drawing);
      for (const Entity &one : drawing.Entities)
      {
        CHECK(index.FindEntity(BaseOf(one).Self) == FindEntity(drawing, BaseOf(one).Self));
      }
      for (const auto &block : drawing.Blocks)
      {
        for (const Entity &one : block.Entities)
        {
          CHECK(index.FindEntity(BaseOf(one).Self) == FindEntity(drawing, BaseOf(one).Self));
        }
      }
      for (const Object &one : drawing.Objects)
      {
        CHECK(index.FindObject(BaseOf(one).Self) == FindObject(drawing, BaseOf(one).Self));
      }
      CHECK(index.FindEntity(Handle{}) == nullptr);
      CHECK(index.FindObject(Handle{.Value = 0xFFFFFF}) == nullptr);
    }
  }
} // namespace DxfIO
