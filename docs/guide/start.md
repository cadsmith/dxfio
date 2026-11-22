---
title: Getting started
---

dxfio is a C++23 library that reads and writes AutoCAD DXF files: every release from R10 to R2018, ASCII and binary, read whole and saved back to the same release or to any other.

## Add it to a project [#add]

dxfio needs CMake 3.28 and a C++23 compiler and standard library, nothing else; the tests and doctest are built only when dxfio is the top level project. It builds as a static library, or as a shared one with `BUILD_SHARED_LIBS=ON`.

```cmake tab="CMakeLists.txt"
add_subdirectory(dxfio)
target_link_libraries(app PRIVATE dxfio::dxfio)
```

```sh tab="Build and test"
cmake --preset debug
cmake --build --preset debug
ctest --preset debug
```

## A first program [#first]

`dxfio/dxfio.hpp` brings in the whole interface, under the namespace `DxfIO`. This program makes a drawing with one line, saves it, and reads it back. A bad file never throws: a load or a save that fails returns its `Failure`, with what went wrong and, for an ASCII file, on which line. Only running out of memory, or of threads to start, can throw.

```cpp title="main.cpp"
#include "dxfio/dxfio.hpp"

#include <print>

auto main() -> int
{
  DxfIO::Drawing drawing = DxfIO::Create(DxfIO::Version::R2018);

  DxfIO::Entities::Line line{};
  line.End = {.X = 100, .Y = 50, .Z = 0};
  if (!DxfIO::Edit::Add(drawing, line)) return 1;

  const auto saved = DxfIO::SaveFile(drawing, "first.dxf");
  if (!saved)
  {
    std::println("{}", saved.error().What);
    return 1;
  }

  const auto loaded = DxfIO::LoadFile("first.dxf");
  if (!loaded)
  {
    std::println("line {}: {}", loaded.error().Line.value_or(0), loaded.error().What);
    return 1;
  }
  std::println("{} entity, {} issues", loaded->Drawing.Entities.size(),
               loaded->Issues.size());
}
```

```text title="Output"
1 entity, 0 issues
```

> [!NOTE]
> Each tutorial is the body of a `main` like this one. `npm run examples`, in `site/`, compiles every one with warnings as errors and runs it: its output must be the one shown, and every file it writes must read back without an issue and pass dxfio's audit; with `-- --acad` it runs AutoCAD 2026's audit on them too.
