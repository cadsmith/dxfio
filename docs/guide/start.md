# Getting started

dxfio is a C++23 library that reads and writes AutoCAD DXF files: every release from R10 to R2018, ASCII and binary, read whole and saved back to the same release or to any other.

## Add it to a project {#add}

dxfio is a static library that needs nothing but the standard library; the tests and doctest are built only when dxfio is the top level project.

::: code-group

```cmake [CMakeLists.txt]
add_subdirectory(dxfio)
target_link_libraries(app PRIVATE dxfio::dxfio)
```

```sh [Build and test]
cmake --preset debug
cmake --build --preset debug
ctest --preset debug
```

:::

## A first program {#first}

`dxfio/dxfio.hpp` brings in the whole interface, under the namespace `DxfIO`. This program makes a drawing with one line, saves it, and reads it back. Nothing throws: a load or a save that fails returns its `Failure`, with what went wrong and, for a load, on which line.

```cpp [main.cpp]
#include "dxfio/dxfio.hpp"

#include <print>

auto main() -> int
{
  DxfIO::Document drawing = DxfIO::Create(DxfIO::Version::R2018);

  DxfIO::Entities::Line line{};
  line.End = {.X = 100, .Y = 50, .Z = 0};
  DxfIO::Edit::Add(drawing, line);

  const auto saved = DxfIO::SaveFile(drawing, "first.dxf");
  if (!saved)
  {
    std::println("{}", saved.error().What);
    return 1;
  }

  const auto loaded = DxfIO::LoadFile("first.dxf");
  if (!loaded)
  {
    std::println("line {}: {}", loaded.error().Line, loaded.error().What);
    return 1;
  }
  std::println("{} entity, {} issues", loaded->Document.Entities.size(),
               loaded->Issues.size());
}
```

```text [Output]
1 entity, 0 issues
```

> [!NOTE]
> Each tutorial is the body of a `main` like this one. Every one is compiled with warnings as errors and run; its output is the one shown, and every file it writes reads back without an issue and passes the audit of AutoCAD 2026.
