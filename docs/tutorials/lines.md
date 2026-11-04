# Lines, circles and arcs

Points, lines, circles, arcs, ellipses and filled solids, drawn in model space of a new drawing.

## A new drawing {#new}

`Create` gives a drawing AutoCAD opens without a finding: the tables, the two space blocks, the layouts and the objects a release needs. `Edit::Add` puts an entity in model space unless told otherwise.

```cpp
DxfIO::Document drawing = DxfIO::Create(DxfIO::Version::R2018);
```

## Points and lines {#lines}

Coordinates are in drawing units, in the world coordinate system for these two.

```cpp
DxfIO::Entities::Point point{};
point.Position = {.X = 0, .Y = 0, .Z = 0};
DxfIO::Edit::Add(drawing, point);

DxfIO::Entities::Line line{};
line.Start = {.X = 0, .Y = 0, .Z = 0};
line.End = {.X = 100, .Y = 50, .Z = 0};
DxfIO::Edit::Add(drawing, line);
```

## Circles and arcs {#circles}

An arc runs counterclockwise from `Start` to `End`, both in degrees.

```cpp
DxfIO::Entities::Circle circle{};
circle.Center = {.X = 150, .Y = 25, .Z = 0};
circle.Radius = 25;
DxfIO::Edit::Add(drawing, circle);

DxfIO::Entities::Arc arc{};
arc.Center = {.X = 220, .Y = 25, .Z = 0};
arc.Radius = 25;
arc.Start = 0;
arc.End = 180;
DxfIO::Edit::Add(drawing, arc);
```

## Ellipses {#ellipses}

`Axis` is the end of the major axis relative to the center, `Ratio` the minor axis over the major. `Start` and `End` are parameters in radians; the default 0 to a full turn closes it.

```cpp
DxfIO::Entities::Ellipse ellipse{};
ellipse.Center = {.X = 300, .Y = 25, .Z = 0};
ellipse.Axis = {.X = 40, .Y = 0, .Z = 0};
ellipse.Ratio = 0.5;
DxfIO::Edit::Add(drawing, ellipse);
```

## Filled solids {#solids}

A SOLID fills three or four corners. AutoCAD fills them in Z order, so a square lists its last two corners swapped.

```cpp
DxfIO::Entities::Solid square{};
square.Corners = {{
    {.X = 360, .Y = 0, .Z = 0},
    {.X = 410, .Y = 0, .Z = 0},
    {.X = 360, .Y = 50, .Z = 0},
    {.X = 410, .Y = 50, .Z = 0},
}};
DxfIO::Edit::Add(drawing, square);
```

## Save {#save}

```cpp
const auto saved = DxfIO::SaveFile(drawing, "lines.dxf");
if (!saved)
{
  std::println("{}", saved.error().What);
  return 1;
}
```
