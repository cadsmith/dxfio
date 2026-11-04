# Color and lineweight

The color, lineweight and visibility every entity carries in its `Record`, set on the lines of a new drawing.

## What an entity starts with {#defaults}

A new entity takes its color and lineweight from its layer, 0 unless it names another: [Layers](/tutorials/layers/) sets them there.

```cpp
DxfIO::Document drawing = DxfIO::Create(DxfIO::Version::R2018);

DxfIO::Entities::Line plain{};
plain.End = {.X = 100, .Y = 0, .Z = 0};
std::println("layer {}, color {}, lineweight {}", plain.Record.Layer, plain.Record.Color, plain.Record.Lineweight);
DxfIO::Edit::Add(drawing, plain);
```

```text [Output]
layer 0, color 256, lineweight -1
```

## Color {#color}

`Color` is an AutoCAD Color Index from 1 to 255, or `Colors::Bylayer` (256) to take the layer's color, or `Colors::Byblock` (0) to take the color of the block insert the entity is drawn in. `Colors::White`, 7, is drawn white on a dark background and black on a light one.

```cpp
DxfIO::Entities::Line indexed{};
indexed.Start = {.X = 0, .Y = 10, .Z = 0};
indexed.End = {.X = 100, .Y = 10, .Z = 0};
indexed.Record.Color = 1;
DxfIO::Edit::Add(drawing, indexed);
```

## True color {#rgb}

`Rgb` adds a true color as 0xRRGGBB, written from R2004 on. A release before R2004 keeps only `Color`, so give it an index near the true color as well.

```cpp
DxfIO::Entities::Line orange{};
orange.Start = {.X = 0, .Y = 20, .Z = 0};
orange.End = {.X = 100, .Y = 20, .Z = 0};
orange.Record.Color = 30;
orange.Record.Rgb = 0xFF8000;
DxfIO::Edit::Add(drawing, orange);
```

## Lineweight {#lineweight}

`Lineweight` is in hundredths of a millimeter, so 50 is 0.50 mm, and is written from R2000 on. `Lineweights::Bylayer` (-1) takes the layer's, `Lineweights::Byblock` (-2) the block insert's, and `Lineweights::Standard` (-3) the drawing's default, which `$LWDEFAULT` gives.

```cpp
DxfIO::Entities::Line heavy{};
heavy.Start = {.X = 0, .Y = 30, .Z = 0};
heavy.End = {.X = 100, .Y = 30, .Z = 0};
heavy.Record.Lineweight = 50;
DxfIO::Edit::Add(drawing, heavy);
```

## Visibility {#hidden}

`Hidden` makes the entity invisible; it stays in the drawing. `Transparency`, from R2004, is empty while the entity takes its layer's.

```cpp
DxfIO::Entities::Line invisible{};
invisible.Start = {.X = 0, .Y = 40, .Z = 0};
invisible.End = {.X = 100, .Y = 40, .Z = 0};
invisible.Record.Hidden = true;
DxfIO::Edit::Add(drawing, invisible);
```

## Save {#save}

```cpp
const auto saved = DxfIO::SaveFile(drawing, "colors.dxf");
if (!saved)
{
  std::println("{}", saved.error().What);
  return 1;
}
```
