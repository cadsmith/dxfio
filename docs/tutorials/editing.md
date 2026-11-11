---
title: Find and change entities
---

Find entities by kind, layer and handle, change and delete some, and save the drawing.

## Load [#load]

The drawing is plan.dxf, the one [Read a drawing](/tutorials/reading/) walks through, loaded as a value this time so it can change.

```cpp
auto loaded = DxfIO::LoadFile("plan.dxf");
if (!loaded)
{
  std::println("line {}: {}", loaded.error().Line, loaded.error().What);
  return 1;
}
DxfIO::Document &drawing = loaded->Document;
std::println("{} entities, {} issues", drawing.Entities.size(), loaded->Issues.size());
```

```text title="Output"
29 entities, 0 issues
```

## By kind [#kind]

`std::get_if` gives a pointer to the entity when the variant holds that kind, and null otherwise. `Record.Paper` tells an entity of paper space from one of model space. `Hex` writes a handle as DXF does.

```cpp
for (DxfIO::Entity &one : drawing.Entities)
{
  DxfIO::Entities::Line *line = std::get_if<DxfIO::Entities::Line>(&one);
  if (line == nullptr || line->Record.Paper)
  {
    continue;
  }
  line->Start.X += 10;
  line->End.X += 10;
  std::println("LINE {} moved to ({}, {}) - ({}, {})", DxfIO::Hex(line->Record.Base.Self), line->Start.X,
               line->Start.Y, line->End.X, line->End.Y);
}
```

```text title="Output"
LINE 2CE moved to (10, 0) - (110, 50)
LINE 2CF moved to (10, 0) - (20, 10)
LINE 2DA moved to (50, 0) - (50, 10)
LINE 2DB moved to (10, 20) - (40, 40)
```

## By layer [#layer]

Every kind dxfio types carries its layer and color in `Record`; an `Unknown` has none, and the `requires` test leaves it out. `Edit::Same` compares names as AutoCAD does, without case. `Colors::Bylayer` gives an entity its layer's color; the circle on walls also has a true color in `Rgb`, cleared with it.

```cpp
for (DxfIO::Entity &one : drawing.Entities)
{
  std::visit(
      [&one](auto &kind) -> void
      {
        if constexpr (requires { kind.Record.Layer; })
        {
          DxfIO::Entities::Record &record = kind.Record;
          if (!DxfIO::Edit::Same(record.Layer.View(), "walls"))
          {
            return;
          }
          std::println("{} {} on walls: color {}, true color 0x{:06X}", DxfIO::TypeOf(one),
                       DxfIO::Hex(record.Base.Self), record.Color, record.Rgb.value_or(0));
          record.Color = DxfIO::Colors::Bylayer;
          record.Rgb.reset();
        }
      },
      one);
}
```

```text title="Output"
CIRCLE 2D0 on walls: color 30, true color 0xFF8000
```

## By handle [#handle]

A handle's `Value` is the number DXF writes in hex. `Edit::FindEntity` looks for it in the ENTITIES section and in every block, and returns a pointer into the document, or null. `std::get_if` gives null for a null pointer too, so one test covers both.

```cpp
DxfIO::Entity *found = DxfIO::Edit::FindEntity(drawing, DxfIO::Handle{.Value = 0x2D5});
if (auto *circle = std::get_if<DxfIO::Entities::Circle>(found))
{
  circle->Radius *= 2;
  std::println("CIRCLE 2D5 radius now {}", circle->Radius);
}
```

```text title="Output"
CIRCLE 2D5 radius now 14
```

## Delete [#delete]

`std::erase_if` takes out of the list every entity the test picks and returns how many. It changes nothing else: a GROUP that lists a deleted entity in its `Members` would still name it. Nothing in plan.dxf names its points.

```cpp
const std::size_t deleted = std::erase_if(drawing.Entities, [](const DxfIO::Entity &one) -> bool
                                          { return std::holds_alternative<DxfIO::Entities::Point>(one); });
std::println("{} points deleted, {} entities left", deleted, drawing.Entities.size());
```

```text title="Output"
3 points deleted, 26 entities left
```

## Save [#save]

```cpp
const auto saved = DxfIO::SaveFile(drawing, "editing.dxf");
if (!saved)
{
  std::println("{}", saved.error().What);
  return 1;
}
```
