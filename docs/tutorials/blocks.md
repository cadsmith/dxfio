---
title: Blocks and attributes
---

A block defined once with its own entities and attribute definitions, then placed twice, once scaled and turned, by inserts that give its attributes their values.

## A block [#block]

`Edit::AddBlock` makes an empty block, with its block record where the drawing has block records. Entities added under the block's name go into it, in the block's own coordinates: an insert puts the block's base point, `Begin.Origin`, at its own position.

```cpp
DxfIO::Document drawing = DxfIO::Create(DxfIO::Version::R2018);
DxfIO::Edit::AddBlock(drawing, "Valve");

DxfIO::Entities::Circle body{};
body.Radius = 5;
DxfIO::Edit::Add(drawing, body, "Valve");

DxfIO::Entities::Line stem{};
stem.Start = {.X = 0, .Y = 5, .Z = 0};
stem.End = {.X = 0, .Y = 12, .Z = 0};
DxfIO::Edit::Add(drawing, stem, "Valve");
```

## Attribute definitions [#attdef]

An ATTDEF names an attribute by its `Tag`, with the `Prompt` shown when the block is inserted and a default value in `Lettering->Value`: an ATTDEF keeps its `Lettering` on the heap, reached with `->`. `Horizontal` 1 and `Vertical` 2 center the number on `Alignment`, the base point here, and `Flags` 1 makes the size invisible.

```cpp
DxfIO::Entities::Attdef number{};
number.Tag = DxfIO::Symbol{"NUMBER"};
number.Prompt = "Valve number";
number.Lettering->Value = "V-0";
number.Lettering->Height = 2.5;
number.Lettering->Horizontal = 1;
number.Vertical = 2;
number.Lettering->Alignment = DxfIO::Vector{.X = 0, .Y = 0, .Z = 0};
DxfIO::Edit::Add(drawing, number, "Valve");

DxfIO::Entities::Attdef size{};
size.Tag = DxfIO::Symbol{"SIZE"};
size.Prompt = "Pipe size";
size.Lettering->Value = "DN50";
size.Lettering->Height = 2.5;
size.Flags = 1;
DxfIO::Edit::Add(drawing, size, "Valve");
```

## The block's flags [#flags]

Bit 2 of `Begin.Flags` says the block has attribute definitions that are not constant. `AddBlock` leaves the flags at 0, so set it once the block has its ATTDEFs.

```cpp
if (DxfIO::Blocks::Block *valve = DxfIO::Edit::FindBlock(drawing, "Valve"))
{
  valve->Begin.Flags = 2;
}
```

## Inserts [#inserts]

An INSERT names its block and puts it at `Position`, scaled along each axis by `Scale` and turned by `Rotation`, in degrees.

```cpp
DxfIO::Entities::Insert first{};
first.Block = DxfIO::Symbol{"Valve"};
first.Position = {.X = 0, .Y = 0, .Z = 0};

DxfIO::Entities::Insert second{};
second.Block = DxfIO::Symbol{"Valve"};
second.Position = {.X = 40, .Y = 0, .Z = 0};
second.Scale = {.X = 2, .Y = 2, .Z = 2};
second.Rotation = 30;
```

## Attribute values [#attribs]

An insert gives each attribute its value in an ATTRIB of the same tag and flags. The insert does not move an ATTRIB: it is drawn where its own `Lettering` says, so give it the definition's text as the insert places it. For these two definitions, both at the base point, that is the insert's position, with the height scaled and the rotation turned.

```cpp
const auto attribute = [](const DxfIO::Entities::Attdef &definition, const DxfIO::Entities::Insert &insert,
                          const std::string &value) -> DxfIO::Entities::Attrib
{
  DxfIO::Entities::Attrib out{};
  out.Tag = definition.Tag;
  out.Flags = definition.Flags;
  out.Vertical = definition.Vertical;
  out.Lettering = *definition.Lettering;
  out.Lettering.Value = value;
  out.Lettering.Height *= insert.Scale.Y;
  out.Lettering.Rotation += insert.Rotation;
  out.Lettering.Position = insert.Position;
  if (out.Lettering.Alignment)
  {
    out.Lettering.Alignment = insert.Position;
  }
  return out;
};
```

## The SEQEND [#seqend]

The ATTRIBs follow their INSERT in the file, up to a SEQEND. `Edit::Add` sets the insert's `Follows`, group 66, which says they are there, gives it its SEQEND, and gives the ATTRIBs and the SEQEND handles of their own.

```cpp
first.Attributes.Children = {attribute(number, first, "V-12"), attribute(size, first, "DN50")};
DxfIO::Edit::Add(drawing, first);

second.Attributes.Children = {attribute(number, second, "V-13"), attribute(size, second, "DN80")};
DxfIO::Edit::Add(drawing, second);
```

## Save [#save]

```cpp
const auto saved = DxfIO::SaveFile(drawing, "blocks.dxf");
if (!saved)
{
  std::println("{}", saved.error().What);
  return 1;
}
```
