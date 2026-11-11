---
title: Home
---

## Install [#install]

```cmake tab="CMake"
add_subdirectory(dxfio)
target_link_libraries(app PRIVATE dxfio::dxfio)
```

```cpp tab="C++"
#include "dxfio/dxfio.hpp"

auto loaded = DxfIO::LoadFile("plan.dxf");
```

## Example [#example]

```cpp tab="Load and edit"
#include "dxfio/dxfio.hpp"

auto loaded = DxfIO::LoadFile("plan.dxf");
if (!loaded)
{
  std::println("line {}: {}", loaded.error().Line, loaded.error().What);
  return 1;
}
DxfIO::Document &drawing = loaded->Document;

DxfIO::Tables::Layer walls{};
walls.Record.Name = "Walls";
DxfIO::Edit::Add(drawing, walls);

DxfIO::Entities::Line line{};
line.Record.Layer = DxfIO::Symbol{"Walls"};
line.End = DxfIO::Vector{.X = 100, .Y = 50, .Z = 0};
DxfIO::Edit::Add(drawing, line);
```

```cpp tab="Save to R12"
const auto older = DxfIO::SaveFile(drawing, "plan-r12.dxf", {.Release = DxfIO::Version::R12});
for (const DxfIO::Issue &issue : older.value_or(std::vector<DxfIO::Issue>{}))
{
  std::println("{}", issue.What);
}
```

```cpp tab="Audit"
const DxfIO::Audit::Report report = DxfIO::Audit::Check(drawing);
std::println("{} errors, {} warnings", report.Errors(), report.Warnings());
```
