---
title: Read a drawing
---

Load a DXF file, tell which release wrote it, and see what its tables and entities hold.

## Load [#load]

`LoadFile` reads a file, ASCII or binary, of any release from R10 to R2018. It fails only when the bytes are not DXF; anything it stepped over, mended or kept as it came is listed in `Issues` with its line. plan.dxf, which AutoCAD 2026 wrote, loads with none.

```cpp
#include "dxfio/dxfio.hpp"

const auto loaded = DxfIO::LoadFile("plan.dxf");
if (!loaded)
{
  std::println("line {}: {}", loaded.error().Line, loaded.error().What);
  return 1;
}
for (const DxfIO::Issue &issue : loaded->Issues)
{
  std::println("line {}: {}", issue.Line, issue.What);
}
const DxfIO::Document &drawing = loaded->Document;
```

## The release [#release]

`Header` holds the header variables in the order the file gave them, each a name and the groups of its value. A group's `Value` is a variant of the type its code calls for: `$ACADVER` has one group 1, the release's code as text. `Release` turns the code into a `Version`, and `Name` gives its short name.

```cpp
for (const DxfIO::Header::Variable &variable : drawing.Header)
{
  if (variable.Name != "$ACADVER")
  {
    continue;
  }
  const std::string &code = std::get<std::string>(variable.Values.front().Value);
  if (const auto release = DxfIO::Release(code))
  {
    std::println("{}, {}", code, DxfIO::Name(*release));
  }
}
```

```text title="Output"
AC1032, R2018
```

## Tables [#tables]

Each table is a member of the document: its `Head`, and its `Records` in the order the file gave them. A layer's `Color` is an AutoCAD Color Index and its `Linetype` the name of a linetype; [Layers](/tutorials/layers/) has the rest.

```cpp
for (const DxfIO::Tables::Layer &layer : drawing.Layer.Records)
{
  std::println("layer {}: color {}, {}", layer.Record.Name, layer.Color, layer.Linetype);
}
```

```text title="Output"
layer 0: color 7, Continuous
layer walls: color 3, Continuous
```

## Entities [#entities]

`Entities` holds the entities of model space and of the active paper space layout, in the order they are drawn; a block's are in its entry of `Blocks`. Each is a `std::variant` of the kinds dxfio types, ending with `Unknown` for the rest, and `TypeOf` gives its DXF name, an `Unknown`'s included.

```cpp
std::map<std::string, int> kinds;
for (const DxfIO::Entity &entity : drawing.Entities)
{
  ++kinds[std::string{DxfIO::TypeOf(entity)}];
}
for (const auto &[kind, count] : kinds)
{
  std::println("{} {}", count, kind);
}
```

```text title="Output"
3 3DFACE
4 ARC
3 CIRCLE
3 ELLIPSE
5 LINE
3 POINT
1 RAY
3 SOLID
2 TRACE
1 VIEWPORT
1 XLINE
```

## Model and paper space [#spaces]

Every kind but `Unknown` has a `Record`: the layer, linetype, color and the rest every entity carries, and `Paper`, true for an entity of paper space. [Find and change entities](/tutorials/editing/) reaches into the fields of each kind.

```cpp
int paper = 0;
for (const DxfIO::Entity &entity : drawing.Entities)
{
  std::visit(
      [&paper](const auto &one)
      {
        if constexpr (requires { one.Record; })
        {
          paper += one.Record.Paper ? 1 : 0;
        }
      },
      entity);
}
std::println("{} of {} in paper space", paper, drawing.Entities.size());
```

```text title="Output"
2 of 29 in paper space
```
