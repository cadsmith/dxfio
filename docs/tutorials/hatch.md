---
title: Hatches
---

A solid fill and a pattern, each over a closed polyline whose outline the hatch keeps a copy of.

## A boundary path [#path]

A hatch holds its boundary itself, as paths. Path flag 2 makes a polyline path, whose `Vertices` carry their bulges in `Weight` when `Bulged` is 1; flag 1 marks it external, and `Closed` 1 closes it.

```cpp
DxfIO::Drawing drawing = DxfIO::Create(DxfIO::Version::R2018);

const auto traced = [](const DxfIO::Entities::LwPolyline &outline) -> DxfIO::Entities::Path
{
  DxfIO::Entities::Path path{};
  path.Flags = 1 | 2;
  path.Bulged = 1;
  path.Closed = 1;
  for (const DxfIO::Entities::LwPolyline::Vertex &one : outline.Vertices)
  {
    path.Vertices.push_back({.Position = one.Position, .Weight = one.Bulge});
  }
  return path;
};
```

## A solid fill [#solid]

A new hatch is a solid fill: the pattern named SOLID, with `Solid` 1 and, as AutoCAD writes it, `Kind` 1 for a predefined pattern. The hatch's fields are in `Data`, a `HatchData` kept on the heap and reached with `->`. `Associative` stays 0: the hatch keeps its own boundary and does not follow the polyline.

```cpp
DxfIO::Entities::LwPolyline square{};
square.Flags = 1;
square.Vertices = {
    {.Position = {.X = 0, .Y = 0, .Z = 0}},
    {.Position = {.X = 40, .Y = 0, .Z = 0}},
    {.Position = {.X = 40, .Y = 40, .Z = 0}},
    {.Position = {.X = 0, .Y = 40, .Z = 0}},
};
if (!DxfIO::Edit::Add(drawing, square)) return 1;

DxfIO::Entities::Hatch solid{};
solid.Data->Paths = {traced(square)};
if (!DxfIO::Edit::Add(drawing, solid)) return 1;
```

## A pattern [#pattern]

A pattern sets `Solid` to 0. A predefined pattern, `Kind` 1, is named in `Name` and drawn at `Angle` degrees and `Scale`. `PatternLines` lists its families of lines as drawn: for ANSI31 at 45 degrees and scale 2, AutoCAD writes one family of lines at 90 degrees, each 6.35 from the next.

```cpp
DxfIO::Entities::LwPolyline arch{};
arch.Flags = 1;
arch.Vertices = {
    {.Position = {.X = 60, .Y = 0, .Z = 0}},
    {.Position = {.X = 100, .Y = 0, .Z = 0}},
    {.Position = {.X = 100, .Y = 30, .Z = 0}, .Bulge = 1},
    {.Position = {.X = 60, .Y = 30, .Z = 0}},
};
if (!DxfIO::Edit::Add(drawing, arch)) return 1;

DxfIO::Entities::Hatch pattern{};
pattern.Data->Name = "ANSI31";
pattern.Data->Solid = 0;
pattern.Data->Kind = 1;
pattern.Data->Angle = 45;
pattern.Data->Scale = 2;
pattern.Data->PatternLines = {{.Angle = 90, .X = 0, .Y = 0, .Run = -6.35, .Rise = 0, .Dashes = {}}};
pattern.Data->Paths = {traced(arch)};
if (!DxfIO::Edit::Add(drawing, pattern)) return 1;
```

## Save [#save]

```cpp
const auto saved = DxfIO::SaveFile(drawing, "hatch.dxf");
if (!saved)
{
  std::println("{}", saved.error().What);
  return 1;
}
```
