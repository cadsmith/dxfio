# Text

Single-line TEXT, justified and in a text style added to the STYLE table, and MTEXT that wraps to a width and carries formatting codes.

## A text style {#style}

A STYLE record names the font and proportions text is drawn with: `Width` is the width of the letters over their height and `Oblique` their slant, in degrees. A `Height` of 0, the default, lets each text give its own.

```cpp
DxfIO::Document drawing = DxfIO::Create(DxfIO::Version::R2018);

DxfIO::Tables::Style narrow{};
narrow.Record.Name = "Narrow";
narrow.Font = "txt.shx";
narrow.Width = 0.8;
narrow.Oblique = 15;
DxfIO::Edit::Add(drawing, narrow);
```

## A line of text {#text}

A TEXT is one line in one style, `Standard` unless it names another. Left on its baseline, the default, it starts at `Lettering.Position`; `Height` is the height of its letters in drawing units.

```cpp
DxfIO::Entities::Text title{};
title.Lettering.Position = {.X = 0, .Y = 0, .Z = 0};
title.Lettering.Height = 2.5;
title.Lettering.Value = "Pump room";
DxfIO::Edit::Add(drawing, title);
```

## Justification {#justification}

`Lettering.Horizontal` takes 0 left, 1 center, 2 right or 4 middle, and the text's own `Vertical` 0 baseline, 1 bottom, 2 middle or 3 top. Each of these but left on the baseline holds the text to `Lettering.Alignment`. `Position` is then where the drawn text starts, which depends on the font and which dxfio does not work out, so these texts give it the alignment point too.

```cpp
struct Justification
{
  int Horizontal;
  int Vertical;
  std::string Value;
};
const std::vector<Justification> justifications{
    {.Horizontal = 1, .Vertical = 0, .Value = "center"},
    {.Horizontal = 2, .Vertical = 0, .Value = "right"},
    {.Horizontal = 4, .Vertical = 0, .Value = "middle"},
    {.Horizontal = 1, .Vertical = 3, .Value = "top center"},
    {.Horizontal = 0, .Vertical = 1, .Value = "bottom left"},
};

double y = 0;
for (const Justification &one : justifications)
{
  DxfIO::Entities::Text text{};
  text.Lettering.Position = {.X = 60, .Y = y, .Z = 0};
  text.Lettering.Alignment = text.Lettering.Position;
  text.Lettering.Height = 2.5;
  text.Lettering.Value = one.Value;
  text.Lettering.Horizontal = one.Horizontal;
  text.Vertical = one.Vertical;
  DxfIO::Edit::Add(drawing, text);
  y += 6;
}
```

## In a style {#styled}

`Lettering.Style` names the style. `Width` and `Oblique` are the text's own, groups 41 and 51, and this one takes the style's.

```cpp
DxfIO::Entities::Text styled{};
styled.Lettering.Position = {.X = 0, .Y = 10, .Z = 0};
styled.Lettering.Height = 2.5;
styled.Lettering.Value = "Narrow and slanted";
styled.Lettering.Style = DxfIO::Symbol{"Narrow"};
styled.Lettering.Width = narrow.Width;
styled.Lettering.Oblique = narrow.Oblique;
DxfIO::Edit::Add(drawing, styled);
```

## Multiline text {#mtext}

An MTEXT wraps its text in a frame `Width` wide, its `Attachment` point, top left by default, at `Position`. Its fields are in `Paragraph`, kept on the heap and reached with `->`. A value of any length goes in `Value` whole: dxfio writes it in pieces of 250 characters.

```cpp
DxfIO::Entities::MText note{};
note.Paragraph->Position = {.X = 0, .Y = 50, .Z = 0};
note.Paragraph->Height = 2.5;
note.Paragraph->Width = 40;
note.Paragraph->Value = R"(Pump P-1 feeds {\C1;tank 3} through valve V-12, open in service\PDrain before winter)";
DxfIO::Edit::Add(drawing, note);
```

## Formatting codes {#codes}

The codes are in the text itself. Saved to R12, which has no MTEXT, the note becomes plain TEXT, a new one only where the note breaks its own line and not where its width wraps it. What is left shows what is code: `\P` ends a line, and a code with a value, such as `\C1;`, runs to its semicolon.

```cpp
DxfIO::Document alone = DxfIO::Create(DxfIO::Version::R2018);
DxfIO::Edit::Add(alone, note);
const auto older = DxfIO::Save(alone, {.Release = DxfIO::Version::R12})
                       .and_then([](const DxfIO::Saved &flat) { return DxfIO::Load(flat.Bytes); });
if (older)
{
  for (const DxfIO::Entity &one : older->Document.Entities)
  {
    if (const auto *line = std::get_if<DxfIO::Entities::Text>(&one))
    {
      std::println("{}", line->Lettering.Value);
    }
  }
}
```

```text [Output]
Pump P-1 feeds tank 3 through valve V-12, open in service
Drain before winter
```

## Save {#save}

```cpp
const auto saved = DxfIO::SaveFile(drawing, "text.dxf");
if (!saved)
{
  std::println("{}", saved.error().What);
  return 1;
}
```
