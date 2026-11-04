# 3D faces

Six 3DFACE entities closing a box in model space of a new drawing, the top one with its edges hidden.

## Corners {#corners}

A 3DFACE is a triangle or a quadrilateral with its corners anywhere in space. `Corners` holds four, in world coordinates, groups 10 to 13 in that order; a triangle repeats its third as its fourth.

```cpp
DxfIO::Document drawing = DxfIO::Create(DxfIO::Version::R2018);

const DxfIO::Corners bottom{{
    {.X = 0, .Y = 0, .Z = 0},
    {.X = 100, .Y = 0, .Z = 0},
    {.X = 100, .Y = 60, .Z = 0},
    {.X = 0, .Y = 60, .Z = 0},
}};
DxfIO::Entities::Face3d floor{};
floor.Corners = bottom;
DxfIO::Edit::Add(drawing, floor);
```

## The sides {#sides}

Unlike a SOLID, a 3DFACE has no normal and no thickness: its corners alone place it, so a side stands upright by their heights.

```cpp
DxfIO::Corners top = bottom;
for (DxfIO::Vector &corner : top)
{
  corner.Z = 40;
}
for (std::size_t at = 0; at < bottom.size(); ++at)
{
  const std::size_t next = (at + 1) % bottom.size();
  DxfIO::Entities::Face3d side{};
  side.Corners = {{bottom.at(at), bottom.at(next), top.at(next), top.at(at)}};
  DxfIO::Edit::Add(drawing, side);
}
```

## Hidden edges {#edges}

`Flags` hides edges, one bit each: 1 the first, 2 the second, 4 the third, 8 the fourth. The top hides all four; each is also the top edge of a side, which still shows it.

```cpp
DxfIO::Entities::Face3d lid{};
lid.Corners = top;
lid.Flags = 1 | 2 | 4 | 8;
DxfIO::Edit::Add(drawing, lid);
```

## Save {#save}

```cpp
const auto saved = DxfIO::SaveFile(drawing, "faces.dxf");
if (!saved)
{
  std::println("{}", saved.error().What);
  return 1;
}
```
