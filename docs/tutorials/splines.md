---
title: Splines
---

Two SPLINE entities in model space of a new drawing: one given by its control points and knots, one by the points it passes through.

## By control points [#poles]

`Poles` are the control points the curve is drawn toward, in world coordinates, and `Degree` its degree. A curve of degree 3 on six control points takes 6 + 3 + 1 = 10 knots; the first and last repeated four times make it start on the first control point and end on the last. Flag 8 says the curve lies in a plane, and `Normal` gives that plane.

```cpp
DxfIO::Document drawing = DxfIO::Create(DxfIO::Version::R2018);

DxfIO::Entities::Spline controlled{};
controlled.Curve->Flags = 8;
controlled.Curve->Normal = DxfIO::Up;
controlled.Curve->Degree = 3;
controlled.Curve->Knots = {0, 0, 0, 0, 1, 2, 3, 3, 3, 3};
controlled.Curve->Poles = {
    {.X = 0, .Y = 0, .Z = 0},  {.X = 20, .Y = 40, .Z = 0},  {.X = 50, .Y = 40, .Z = 0},
    {.X = 70, .Y = 0, .Z = 0}, {.X = 100, .Y = 0, .Z = 0}, {.X = 120, .Y = 40, .Z = 0},
};
DxfIO::Edit::Add(drawing, controlled);
```

## Through fit points [#fits]

`Fits` lists the points the curve passes through, and `Fitted` how far from them it may pass. Flags 1064 copy what AutoCAD writes on such a spline in its own files: 8 for planar, with bits 32 and 1024 beside it. dxfio writes the lists as they are and works out no knots or control points from the fit points, so this spline is saved with none.

```cpp
DxfIO::Entities::Spline fitted{};
fitted.Curve->Flags = 1064;
fitted.Curve->Normal = DxfIO::Up;
fitted.Curve->Degree = 3;
fitted.Curve->Fitted = 1e-10;
fitted.Curve->Fits = {
    {.X = 0, .Y = 80, .Z = 0},
    {.X = 40, .Y = 120, .Z = 0},
    {.X = 80, .Y = 80, .Z = 0},
    {.X = 120, .Y = 120, .Z = 0},
};
DxfIO::Edit::Add(drawing, fitted);
```

## Save [#save]

```cpp
const auto saved = DxfIO::SaveFile(drawing, "splines.dxf");
if (!saved)
{
  std::println("{}", saved.error().What);
  return 1;
}
```
