# dxfio

A C++23 library that reads and writes AutoCAD DXF files: every release from
R10 to R2018, ASCII and binary, read whole and saved back to the same release
or to any other.

Its guide, tutorials and reference: <https://dxfio.dev>.

```cpp
#include "dxfio/dxfio.hpp"

auto loaded = DxfIO::LoadFile("plan.dxf");
if (!loaded)
{
  return loaded.error();
}
DxfIO::Entities::Line line{};
line.End = DxfIO::Vector{.X = 100, .Y = 50, .Z = 0};
DxfIO::Edit::Add(loaded->Document, line);
const auto saved = DxfIO::SaveFile(loaded->Document, "plan-r2010.dxf",
                                   {.Release = DxfIO::Version::R2010});
```

What it keeps, the kinds it types, how it saves to other releases, its audit
and its limits: [docs/dxf.md](docs/dxf.md).

## Use in a project

```cmake
add_subdirectory(dxfio)
target_link_libraries(app PRIVATE dxfio::dxfio)
```

It needs nothing but the standard library: tests and doctest only when dxfio is
the top level project. That library must read a `double` with `std::from_chars`:
libc++ 20, libstdc++ 11 and MSVC 19.24 (Visual Studio 2019 16.4) are the first
that do.

## Build and test

```sh
cmake --preset debug
cmake --build --preset debug
ctest --preset debug
```

| Preset | Purpose |
| :--- | :--- |
| `debug` | Development build and tests |
| `release` | Optimized build |
| `sanitize` | ASan and UBSan, at `-O1` |
| `thread` | TSan, at `-O1` |
| `memory` | MSan, at `-O1`; Linux only, needs a libc++ built with MSan |
| `coverage` | LLVM coverage at `-O1`, `--target coverage` fails below 100% of lines, functions, regions and branches |
| `tidy` | Builds the library through clang-tidy, every finding an error |
| `fuzz` | libFuzzer targets under `build/fuzz/bin`, needs LLVM clang on the path |

`ctest` runs `dxfio_tests` once per test file, named `dxfio/<file>`, so
`ctest --preset debug -j 8` spreads them and `-R dxfio/tags/` runs one area.

## Continuous integration

[ci.yml](.github/workflows/ci.yml) runs on every pull request and push to
`main`: clang-format, the CMake guidelines, the public interface's snapshot
and the pull request's title; the `release` preset built and tested with GCC,
Clang, Apple Clang and MSVC; `sanitize`, `thread`, `memory` and `coverage`
with Clang 23; `tidy` with clang-tidy 23; and every fuzz target for a minute on the
corpus the nightly runs grow. Each job keeps a ccache that a push to `main`
saves, so a pull request compiles only what it changed.
[fuzz.yml](.github/workflows/fuzz.yml) fuzzes for twenty minutes a target
every night and keeps what it finds, and
[codeql.yml](.github/workflows/codeql.yml) runs CodeQL's security and quality
queries.

## Layout

| Directory | Holds |
| :--- | :--- |
| `include/dxfio` | The public headers, namespace `DxfIO`; `dxfio/dxfio.hpp` brings in all of them |
| `source` | The implementation and the layout of each kind |
| `tests/dxfio` | Unit tests |
| `tests/api` | The public interface's snapshot, and a check that each header compiles alone |
| `tests/fixtures/acad` | Drawings AutoCAD 2026 wrote for each release |
| `tests/fuzz` | Fuzz targets, their corpus and the inputs they found |
| `tools/acad` | The AutoCAD console scripts that made the fixtures, and a verifier |
| `tools/dxf` | The generator of the code page tables |
| `vendor` | Where doctest is fetched from |
| `docs` | The documentation's pages, notes on DXF and on dxfio, and the guidelines |
| `site` | The documentation website that renders `docs`, its examples built and run by `npm run examples` |

## Guidelines

[C++](docs/cpp.md), [commits](docs/commit.md) and [CMake](docs/cmake.md).

## The public interface

Every public declaration has a `///` comment, and every header compiles on
its own. `tests/api/snapshot.txt` holds the public headers without their
comments; the `dxfio_api` test fails when they change, so a change to the
interface is always a deliberate one. When it is, take the new snapshot:

```sh
cmake --build --preset debug --target dxfio_api
```

## Documentation site

`site/` is the documentation site. Its pages are the Markdown in
`docs/guide/`, `docs/tutorials/` and `docs/concepts/`, and its reference is
read from the headers' comments: `npm install`, then `npm run dev` or
`npm run build`.

## License

[MIT](LICENSE)
