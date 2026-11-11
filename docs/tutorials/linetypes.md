---
title: Linetypes
---

Define a dash-dot linetype in the LTYPE table, then draw with it through a layer and on one entity.

## A new drawing [#new]

A new drawing's LTYPE table holds ByBlock, ByLayer and Continuous, none with a pattern.

```cpp
DxfIO::Document drawing = DxfIO::Create(DxfIO::Version::R2018);
for (const DxfIO::Tables::Ltype &ltype : drawing.Ltype.Records)
{
  std::println("{}: {} elements", ltype.Record.Name, ltype.Dashes.size());
}
```

```text title="Output"
ByBlock: 0 elements
ByLayer: 0 elements
Continuous: 0 elements
```

## The pattern [#pattern]

`Dashes` is the pattern, repeated along the line: a positive `Length` is a dash, 0 a dot, a negative length a space; a `Flags` of 0 keeps an element plain, 2 draws `Text` on it and 4 a shape. The linetype's own `Length` is the whole pattern's, the lengths added without their sign: 10 + 3 + 0 + 3. `Edit::Add` fails when the table holds a linetype of that name.

```cpp
DxfIO::Tables::Ltype dashdot{};
dashdot.Record.Name = "DashDot";
dashdot.Description = "__ . __ . __ . __";
dashdot.Length = 16;
dashdot.Dashes = {{.Length = 10}, {.Length = -3}, {.Length = 0}, {.Length = -3}};
DxfIO::Edit::Add(drawing, dashdot);
```

## Through a layer [#layer]

A layer's `Linetype` names the linetype of the entities on it whose own is ByLayer, as a new entity's is.

```cpp
DxfIO::Tables::Layer axes{};
axes.Record.Name = "Axes";
axes.Color = 1;
axes.Linetype = DxfIO::Symbol{"DashDot"};
DxfIO::Edit::Add(drawing, axes);

DxfIO::Entities::Line axis{};
axis.Record.Layer = DxfIO::Symbol{"Axes"};
axis.Start = {.X = -50, .Y = 0, .Z = 0};
axis.End = {.X = 250, .Y = 0, .Z = 0};
DxfIO::Edit::Add(drawing, axis);
```

## On one entity [#entity]

An entity that names a linetype in `Record.Linetype` is drawn with it, whatever its layer's. `Edit::Add` does not check the name: `Audit::Check` reports one the table does not hold, and `Audit::Repair` sets it to ByLayer. `Record.Scale` scales the pattern for that entity alone: at 2, each dash and space is twice as long.

```cpp
DxfIO::Entities::Circle hole{};
hole.Center = {.X = 100, .Y = 0, .Z = 0};
hole.Radius = 60;
hole.Record.Linetype = DxfIO::Symbol{"DashDot"};
hole.Record.Scale = 2;
DxfIO::Edit::Add(drawing, hole);
```

## Save [#save]

```cpp
const auto saved = DxfIO::SaveFile(drawing, "linetypes.dxf");
if (!saved)
{
  std::println("{}", saved.error().What);
  return 1;
}
```
