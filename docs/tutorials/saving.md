---
title: Save a drawing
---

One drawing saved to R2000 and to R12 with the issues each save reports, then as binary DXF, and its text in a code page.

## The drawing [#load]

The drawing here, `plan.dxf`, is one AutoCAD saved as R2018. Among its entities are three ellipses, a ray, an xline and the viewport of its first layout.

```cpp
const auto plan = DxfIO::LoadFile("plan.dxf");
if (!plan)
{
  std::println("line {}: {}", plan.error().Line, plan.error().What);
  return 1;
}
const DxfIO::Document &drawing = plan->Document;
```

## Print what a save reports [#issues]

`SaveFile` writes the file and returns the issues of the save: what the release could not hold, and how it was kept or left out. An issue names the handle of the record it concerns, or none when it concerns no one record.

```cpp
const auto told = [](const std::vector<DxfIO::Issue> &issues)
{
  for (const DxfIO::Issue &issue : issues)
  {
    if (issue.Record)
    {
      std::println("  {}: {}", DxfIO::Hex(issue.Record), issue.What);
    }
    else
    {
      std::println("  {}", issue.What);
    }
  }
};
```

## R2000 [#r2000]

`Options::Release` picks the release. R2000 holds every entity of this drawing, but not its visual styles, whose layout changes at R2010 and again at R2013: they are left out, and what names them is cleared.

```cpp
const auto r2000 = DxfIO::SaveFile(drawing, "saving-r2000.dxf", {.Release = DxfIO::Version::R2000});
if (!r2000)
{
  std::println("{}", r2000.error().What);
  return 1;
}
std::println("R2000:");
told(*r2000);
```

```text title="Output"
R2000:
  visual styles left out, R2000 holds them otherwise
```

## R12 [#r12]

For R12 an ellipse becomes a 3D POLYLINE along its curve, and a ray or an xline a LINE cut at the drawing's extents. The paper space viewport is left out, where AutoCAD would keep it with its settings in ACAD MVIEW extended data. R12 also keeps no objects and no layout but the one drawn in `*Paper_Space`, and neither loss is among the issues. [Releases](/concepts/releases/) lists what each release lacks.

```cpp
const auto r12 = DxfIO::SaveFile(drawing, "saving-r12.dxf", {.Release = DxfIO::Version::R12});
if (!r12)
{
  std::println("{}", r12.error().What);
  return 1;
}
std::println("R12:");
told(*r12);
```

```text title="Output"
R12:
  2F0: ELLIPSE drawn as POLYLINE for R12
  2F1: ELLIPSE drawn as POLYLINE for R12
  2F2: ELLIPSE drawn as POLYLINE for R12
  2F3: RAY drawn as LINE for R12
  2F4: XLINE drawn as LINE for R12
  2E5: VIEWPORT left out, R12 has no such entity
```

## Binary, in memory [#binary]

`Save` returns the file in `Saved::Bytes` with its issues, instead of writing it; another overload writes to any `std::ostream`. `Binary` writes binary DXF, which AutoCAD reads as it reads ASCII.

```cpp
const auto ascii = DxfIO::Save(drawing);
const auto binary = DxfIO::Save(drawing, {.Binary = true});
if (ascii && binary)
{
  std::println("ASCII {} bytes, binary {} bytes", ascii->Bytes.size(), binary->Bytes.size());
}
```

```text title="Output"
ASCII 129595 bytes, binary 67615 bytes
```

## In a code page [#codepage]

Before R2007 a file's text is in a code page: the drawing's own, `$DWGCODEPAGE`, unless `Codepage` names another. A character the page lacks is written as a `\U+` escape: Ø is not in ANSI 1251, the Cyrillic page. `Release` is R2018 unless set, whatever the drawing's own, and R2018 writes UTF-8, so name the release too.

```cpp
DxfIO::Document note = DxfIO::Create(DxfIO::Version::R2004);
DxfIO::Entities::Text text{};
text.Lettering.Height = 2.5;
text.Lettering.Value = "Ø 20";
DxfIO::Edit::Add(note, text);

const auto cyrillic = DxfIO::Save(note, {.Release = DxfIO::Version::R2004, .Codepage = "ANSI_1251"});
if (cyrillic)
{
  const std::string_view bytes = cyrillic->Bytes;
  const std::size_t at = bytes.find("\\U+");
  if (at != std::string_view::npos)
  {
    std::println("{}", bytes.substr(at, bytes.find_first_of("\r\n", at) - at));
  }
}
```

```text title="Output"
\U+00D8 20
```
