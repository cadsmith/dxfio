---
title: Polylines
---

Lightweight polylines with widths, arcs and a closed outline, and a 3D polyline whose vertices follow it in the file.

## A lightweight polyline [#lwpolyline]

An LWPOLYLINE holds its vertices itself, in its own coordinate system. Only the X and Y of a vertex are written, groups 10 and 20; `Elevation` places all of them along the normal.

```cpp
DxfIO::Drawing drawing = DxfIO::Create(DxfIO::Version::R2018);

DxfIO::Entities::LwPolyline path{};
path.Vertices = {
    {.Position = {.X = 0, .Y = 0, .Z = 0}},
    {.Position = {.X = 30, .Y = 0, .Z = 0}},
    {.Position = {.X = 30, .Y = 20, .Z = 0}},
};
if (!DxfIO::Edit::Add(drawing, path)) return 1;
```

## Widths [#widths]

`Width` is a width every segment shares. When they differ, empty it and give each vertex its `Widths`: the segment from that vertex starts at `Start` and ends at `End`.

```cpp
DxfIO::Entities::LwPolyline arrow{};
arrow.Width = std::nullopt;
arrow.Vertices = {
    {.Position = {.X = 0, .Y = 30, .Z = 0}, .Widths = DxfIO::Widths{.Start = 1, .End = 1}},
    {.Position = {.X = 24, .Y = 30, .Z = 0}, .Widths = DxfIO::Widths{.Start = 5, .End = 0}},
    {.Position = {.X = 30, .Y = 30, .Z = 0}, .Widths = DxfIO::Widths{.Start = 0, .End = 0}},
};
if (!DxfIO::Edit::Add(drawing, arrow)) return 1;
```

## Arcs and a closed outline [#arcs]

A vertex's `Bulge` makes the segment from it an arc: the tangent of a quarter of the arc's angle, so 1 is a half circle and 0 a straight segment. `Flags` 1, `LwPolylineFlags::Closed`, closes the polyline with a segment from the last vertex back to the first, which takes the last vertex's bulge and widths. `DxfIO::Has(slot.Flags, DxfIO::Entities::LwPolylineFlags::Closed)` tests it.

```cpp
DxfIO::Entities::LwPolyline slot{};
slot.Flags = 1;
slot.Vertices = {
    {.Position = {.X = 0, .Y = 50, .Z = 0}},
    {.Position = {.X = 30, .Y = 50, .Z = 0}, .Bulge = 1},
    {.Position = {.X = 30, .Y = 60, .Z = 0}},
    {.Position = {.X = 0, .Y = 60, .Z = 0}, .Bulge = 1},
};
if (!DxfIO::Edit::Add(drawing, slot)) return 1;
```

## A 3D polyline [#polyline]

A POLYLINE keeps its vertices as VERTEX entities that follow it in the file. `Flags` 8, `PolylineFlags::Polyline3d`, makes it a 3D polyline: its vertices are in world coordinates, and `Edit::Add` gives each the flag 32 a 3D polyline's vertex takes.

```cpp
const std::vector<DxfIO::Vector> route{
    {.X = 0, .Y = 80, .Z = 0},
    {.X = 20, .Y = 80, .Z = 5},
    {.X = 20, .Y = 100, .Z = 10},
    {.X = 40, .Y = 100, .Z = 10},
};

DxfIO::Entities::Polyline pipe{};
pipe.Flags = 8;
for (const DxfIO::Vector &at : route)
{
  DxfIO::Entities::Vertex vertex{};
  vertex.Position = at;
  pipe.Vertices.Children.push_back(vertex);
}
```

## The SEQEND [#seqend]

The vertices end with a SEQEND. `Edit::Add` gives the polyline its SEQEND, and gives the vertices and the SEQEND handles of their own.

```cpp
if (!DxfIO::Edit::Add(drawing, pipe)) return 1;
```

## Save [#save]

```cpp
const auto saved = DxfIO::SaveFile(drawing, "polylines.dxf");
if (!saved)
{
  std::println("{}", saved.error().What);
  return 1;
}
```
