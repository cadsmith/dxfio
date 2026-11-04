# Units

Reading and setting the units of a drawing, which live in its HEADER section as variables.

## Header variables {#header}

`Document::Header` holds every variable of the HEADER section in file order: its name, such as `$INSUNITS`, and the groups of its value. A small lookup finds one by name.

```cpp
DxfIO::Document drawing = DxfIO::Create(DxfIO::Version::R2018);

auto variable = [](DxfIO::Document &document, const char *name) -> DxfIO::Header::Variable * {
  for (DxfIO::Header::Variable &one : document.Header)
  {
    if (one.Name == name)
    {
      return &one;
    }
  }
  return nullptr;
};
```

## Insertion units {#insunits}

`$INSUNITS` is a whole number naming the unit of the drawing, which AutoCAD uses to scale blocks, images and xrefs inserted from a drawing of another unit. Among its values: 0 unitless, 1 inches, 2 feet, 4 millimeters, 5 centimeters, 6 meters ([AutoCAD's INSUNITS reference](https://help.autodesk.com/cloudhelp/2025/ENU/AutoCAD-Core/files/GUID-A58A87BB-482B-4042-A00A-EEF55A2B4FD8.htm) lists them all). A drawing from `Create` starts in millimeters.

```cpp
DxfIO::Header::Variable *units = variable(drawing, "$INSUNITS");
std::println("$INSUNITS was {}", std::get<std::int64_t>(units->Values.front().Value));
units->Values.front().Value = std::int64_t{6};
```

```text [Output]
$INSUNITS was 4
```

## How lengths are shown {#lunits}

`$LUNITS` is the format AutoCAD shows lengths in: 1 scientific, 2 decimal, 3 engineering, 4 architectural, 5 fractional ([AutoCAD's LUNITS reference](https://help.autodesk.com/cloudhelp/2016/ENU/AutoCAD-LT/files/GUID-D7C80D1F-B1C0-44A9-898E-B3100FF391CB.htm)). `$LUPREC` is how many decimal places. Neither changes a coordinate in the file.

```cpp
variable(drawing, "$LUNITS")->Values.front().Value = std::int64_t{2};
variable(drawing, "$LUPREC")->Values.front().Value = std::int64_t{2};
```

## Older releases {#older}

`$INSUNITS` came with R2000: a drawing made for R12 has none, and saving to R12 leaves it out.

```cpp
DxfIO::Document old = DxfIO::Create(DxfIO::Version::R12);
std::println("an R12 drawing has $INSUNITS: {}", variable(old, "$INSUNITS") != nullptr);
```

```text [Output]
an R12 drawing has $INSUNITS: false
```

## Save {#save}

```cpp
const auto saved = DxfIO::SaveFile(drawing, "units.dxf");
if (!saved)
{
  std::println("{}", saved.error().What);
  return 1;
}
```
