# Paper space

Entities drawn on a layout's sheet rather than in the model: in paper space of a new drawing, then in the second layout of a drawing AutoCAD saved.

## Paper space {#paper}

`Create` makes one paper space layout, Layout1, whose sheet is the `*Paper_Space` block and whose limits run from 0,0 to 12,9. `Space::Paper` adds an entity to it: the entity goes to the ENTITIES section with its `Paper` flag set, owned by the `*Paper_Space` block record.

```cpp
DxfIO::Document drawing = DxfIO::Create(DxfIO::Version::R2018);

DxfIO::Entities::LwPolyline border{};
border.Flags = 1;
border.Vertices = {
    {.Position = {.X = 0.5, .Y = 0.5, .Z = 0}},
    {.Position = {.X = 11.5, .Y = 0.5, .Z = 0}},
    {.Position = {.X = 11.5, .Y = 8.5, .Z = 0}},
    {.Position = {.X = 0.5, .Y = 8.5, .Z = 0}},
};
DxfIO::Edit::Add(drawing, border, DxfIO::Edit::Space::Paper);

DxfIO::Entities::Text title{};
title.Lettering.Position = {.X = 1, .Y = 1, .Z = 0};
title.Lettering.Height = 0.25;
title.Lettering.Value = "Ground floor";
DxfIO::Edit::Add(drawing, title, DxfIO::Edit::Space::Paper);
```

## Another layout {#layout}

A drawing's other layouts each draw their sheet in a block of their own, and a LAYOUT object names its block's record in `Block`. `plan.dxf` here is a drawing AutoCAD saved with two layouts; the loop finds the block of Layout2.

```cpp
auto plan = DxfIO::LoadFile("plan.dxf");
if (!plan)
{
  std::println("line {}: {}", plan.error().Line, plan.error().What);
  return 1;
}
DxfIO::Document &sheets = plan->Document;

std::string sheet;
for (const DxfIO::Object &one : sheets.Objects)
{
  const auto *layout = std::get_if<DxfIO::Objects::Layout>(&one);
  if (layout == nullptr || layout->Name != "Layout2")
  {
    continue;
  }
  for (const DxfIO::Tables::BlockRecord &record : sheets.BlockRecord.Records)
  {
    if (record.Record.Base.Self == layout->Block)
    {
      sheet = record.Record.Name;
    }
  }
}
std::println("Layout2 is drawn in {}", sheet);
```

```text [Output]
Layout2 is drawn in *Paper_Space0
```

## Add to its block {#block}

`Edit::Add` with the block's name puts the entity in that block, owned by its block record. The block's BLOCK record carries the paper space flag, so the entity takes it too.

```cpp
DxfIO::Entities::Text stamp{};
stamp.Lettering.Position = {.X = 1, .Y = 1, .Z = 0};
stamp.Lettering.Height = 0.25;
stamp.Lettering.Value = "Details";
const auto added = DxfIO::Edit::Add(sheets, stamp, sheet);
if (!added)
{
  std::println("{}", added.error().What);
  return 1;
}
```

## Save {#save}

```cpp
const auto saved = DxfIO::SaveFile(drawing, "paper.dxf");
if (!saved)
{
  std::println("{}", saved.error().What);
  return 1;
}
const auto resaved = DxfIO::SaveFile(sheets, "paper-plan.dxf");
if (!resaved)
{
  std::println("{}", resaved.error().What);
  return 1;
}
```
