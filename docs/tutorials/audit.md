# Audit and repair

A drawing broken on purpose, what `Audit::Check` finds in it, and what `Audit::Repair` fixes before it is saved. A drawing made with `Create` and `Edit::Add` passes the check as it is; the audit is for drawings other programs wrote, and for mistakes like these.

## Break a drawing {#break}

`Edit::Add` gives an entity a new handle only when it has none, and adds it whatever layer, linetype or block it names. That is enough to break a new drawing four ways: a circle takes the handle of a line, a line names a layer and a linetype the drawing lacks, and an insert places a block that is not there.

```cpp
DxfIO::Document drawing = DxfIO::Create(DxfIO::Version::R2018);

DxfIO::Entities::Line line{};
line.End = {.X = 100, .Y = 0, .Z = 0};
const auto first = DxfIO::Edit::Add(drawing, line);

DxfIO::Entities::Circle circle{};
circle.Record.Base.Self = *first;
circle.Center = {.X = 50, .Y = 50, .Z = 0};
circle.Radius = 20;
DxfIO::Edit::Add(drawing, circle);

DxfIO::Entities::Line wall{};
wall.Record.Layer = DxfIO::Symbol{"Walls"};
wall.Record.Linetype = DxfIO::Symbol{"DASHED"};
wall.End = {.X = 0, .Y = 100, .Z = 0};
DxfIO::Edit::Add(drawing, wall);

DxfIO::Entities::Insert door{};
door.Block = DxfIO::Symbol{"Door"};
door.Position = {.X = 100, .Y = 100, .Z = 0};
DxfIO::Edit::Add(drawing, door);
```

## Check {#check}

`Audit::Check` leaves the drawing as it is and lists its findings in the order the checks ran: handles, required records, references, meshes, SEQENDs. An error is a finding AutoCAD would refuse or recover the drawing for; with warnings alone, it opens the drawing as it is. `Clean` is false while an error is left unfixed.

```cpp
const auto listed = [](const DxfIO::Audit::Report &report)
{
  for (const DxfIO::Audit::Finding &finding : report.Findings)
  {
    std::println("  {} {}{}", finding.Level == DxfIO::Audit::Severity::Error ? "error  " : "warning", finding.What,
                 finding.Fixed ? ", fixed" : "");
  }
  std::println("  clean: {}", report.Clean());
};

std::println("Check:");
listed(DxfIO::Audit::Check(drawing));
```

```text [Output]
Check:
  error   handle 3D of a CIRCLE used twice
  error   layer Walls missing
  warning linetype DASHED missing
  error   block Door missing, its insert left out
  clean: false
```

## Repair {#repair}

`Audit::Repair` runs the same checks and fixes what they find, in place. The circle, the second to hold the handle, takes a new one; the layer is made; the linetype falls back to ByLayer; the insert of the missing block is left out. `$HANDSEED` then moves past the handles in use, without a finding.

> [!IMPORTANT]
> `Edit::Add` takes new handles from `$HANDSEED`. A drawing another program wrote may carry a seed behind its own handles: run `Audit::Repair` on it once before you add to it.

```cpp
std::println("Repair:");
listed(DxfIO::Audit::Repair(drawing));

for (const DxfIO::Entity &one : drawing.Entities)
{
  std::println("  {} {}", DxfIO::TypeOf(one), DxfIO::Hex(DxfIO::BaseOf(one).Self));
}
```

```text [Output]
Repair:
  error   handle 3D of a CIRCLE used twice, fixed
  error   layer Walls missing, fixed
  warning linetype DASHED missing, fixed
  error   block Door missing, its insert left out, fixed
  clean: true
  LINE 3D
  CIRCLE 40
  LINE 3E
```

## Save {#save}

The repaired drawing is the one saved, and a second check finds nothing in it.

```cpp
std::println("Check again: {} findings", DxfIO::Audit::Check(drawing).Findings.size());

const auto saved = DxfIO::SaveFile(drawing, "audit.dxf");
if (!saved)
{
  std::println("{}", saved.error().What);
  return 1;
}
```

```text [Output]
Check again: 0 findings
```
