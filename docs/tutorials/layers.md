---
title: Layers
---

Add layers with their own color and lineweight, put entities on them, and turn a layer off, freeze it or lock it.

## A new drawing [#new]

A new drawing has one layer, 0, and a new entity is on it.

```cpp
DxfIO::Document drawing = DxfIO::Create(DxfIO::Version::R2018);
for (const DxfIO::Tables::Layer &layer : drawing.Layer.Records)
{
  std::println("{}: color {}, linetype {}, lineweight {}", layer.Record.Name, layer.Color, layer.Linetype,
               layer.Lineweight);
}
```

```text title="Output"
0: color 7, linetype Continuous, lineweight -3
```

## Add a layer [#add]

A `Tables::Layer` starts as layer 0 is: white (color 7), Continuous, plotted, at the drawing's default lineweight (-3). `Color` is an AutoCAD Color Index from 1 to 255 and `Lineweight` is in hundredths of a millimeter. To give a layer a linetype other than Continuous, add the linetype to the LTYPE table: see [Linetypes](/tutorials/linetypes/).

```cpp
DxfIO::Tables::Layer walls{};
walls.Record.Name = "Walls";
walls.Color = 1;
walls.Lineweight = 50;
DxfIO::Edit::Add(drawing, walls);

DxfIO::Tables::Layer doors{};
doors.Record.Name = "Doors";
doors.Color = 3;
DxfIO::Edit::Add(drawing, doors);
```

## Names [#names]

`Edit::Add` fails when the table already holds a layer of that name. Names compare as AutoCAD compares them, without case.

```cpp
DxfIO::Tables::Layer again{};
again.Record.Name = "WALLS";
const auto added = DxfIO::Edit::Add(drawing, again);
if (!added)
{
  std::println("{}", added.error().What);
}
```

```text title="Output"
a record named WALLS is there already
```

## Entities on a layer [#entities]

`Record.Layer` names the entity's layer. A color and lineweight left at ByLayer, as a new entity has them, are the layer's. `Edit::Add` does not check that the layer exists; `Audit::Check` reports a layer an entity names that the table does not hold, and `Audit::Repair` makes it.

```cpp
DxfIO::Entities::Line wall{};
wall.Record.Layer = DxfIO::Symbol{"Walls"};
wall.End = {.X = 400, .Y = 0, .Z = 0};
DxfIO::Edit::Add(drawing, wall);

DxfIO::Entities::Arc door{};
door.Record.Layer = DxfIO::Symbol{"Doors"};
door.Center = {.X = 100, .Y = 0, .Z = 0};
door.Radius = 90;
door.End = 90;
DxfIO::Edit::Add(drawing, door);
```

## Off, frozen, locked [#state]

A layer is off when its color is negative: -8 is color 8, off. `Record.Flags` holds the rest as bits: 1 frozen, 2 frozen in new viewports, 4 locked. `Plot` false keeps a layer out of plots.

```cpp
DxfIO::Tables::Layer grid{};
grid.Record.Name = "Grid";
grid.Color = -8;
grid.Plot = false;
DxfIO::Edit::Add(drawing, grid);

DxfIO::Tables::Layer survey{};
survey.Record.Name = "Survey";
survey.Color = 5;
survey.Record.Flags = 1 | 4;
DxfIO::Edit::Add(drawing, survey);
```

## Find and change [#find]

`Edit::Find` gives a pointer to the layer of that name, found without case, or null.

```cpp
if (DxfIO::Tables::Layer *found = DxfIO::Edit::Find(drawing.Layer, "grid"))
{
  found->Color = -found->Color;
  std::println("{} is on, color {}", found->Record.Name, found->Color);
}
```

```text title="Output"
Grid is on, color 8
```

## Save [#save]

```cpp
const auto saved = DxfIO::SaveFile(drawing, "layers.dxf");
if (!saved)
{
  std::println("{}", saved.error().What);
  return 1;
}
```
