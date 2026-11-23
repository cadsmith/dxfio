# dxfio

[![CI](https://github.com/cadsmith/dxfio/actions/workflows/ci.yml/badge.svg?branch=main)](https://github.com/cadsmith/dxfio/actions/workflows/ci.yml)
[![CodeQL](https://github.com/cadsmith/dxfio/actions/workflows/codeql.yml/badge.svg?branch=main)](https://github.com/cadsmith/dxfio/actions/workflows/codeql.yml)
[![Fuzz](https://github.com/cadsmith/dxfio/actions/workflows/fuzz.yml/badge.svg)](https://github.com/cadsmith/dxfio/actions/workflows/fuzz.yml)
[![Coverage](https://coveralls.io/repos/github/cadsmith/dxfio/badge.svg?branch=main)](https://coveralls.io/github/cadsmith/dxfio?branch=main)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/8da3742c6d824e7fbd565dcb9216e041)](https://app.codacy.com/gh/cadsmith/dxfio/dashboard?utm_source=gh&utm_medium=referral&utm_content=&utm_campaign=Badge_grade)
[![C++23](https://img.shields.io/badge/C%2B%2B-23-blue)](docs/cpp.md)
[![License: MIT](https://img.shields.io/badge/license-MIT-blue)](LICENSE)
[![Docs](https://img.shields.io/badge/docs-dxfio.dev-blue)](https://dxfio.dev)

A C++23 library that reads and writes AutoCAD DXF files: every release from
R10 to R2018, ASCII and binary, read whole and saved back to the same release
or to any other.

Its guide, tutorials and reference: <https://dxfio.dev>.

```cpp
#include "dxfio/dxfio.hpp"

#include <print>

auto main() -> int
{
  auto loaded = DxfIO::LoadFile("plan.dxf");
  if (!loaded)
  {
    std::println("{}", loaded.error().What);
    return 1;
  }
  DxfIO::Entities::Line line{};
  line.End = DxfIO::Vector{.X = 100, .Y = 50, .Z = 0};
  if (const auto added = DxfIO::Edit::Add(loaded->Drawing, line); !added)
  {
    std::println("{}", added.error().What);
    return 1;
  }
  const auto saved = DxfIO::SaveFile(loaded->Drawing, "plan-r2010.dxf",
                                     {.Release = DxfIO::Version::R2010});
  return saved ? 0 : 1;
}
```

What it keeps, the kinds it types, how it saves to other releases, its audit
and its limits: [docs/dxf.md](docs/dxf.md).

## Use in a project

As a subdirectory of the project:

```cmake
add_subdirectory(dxfio)
target_link_libraries(app PRIVATE dxfio::dxfio)
```

Or installed, then found:

```sh
cmake --preset release
cmake --build --preset release
cmake --install build/release --prefix /some/prefix
```

```cmake
find_package(dxfio CONFIG REQUIRED)
target_link_libraries(app PRIVATE dxfio::dxfio)
```

```sh
cmake -S . -B build -DCMAKE_PREFIX_PATH=/some/prefix
```

Or prebuilt: each [release](https://github.com/cadsmith/dxfio/releases)
carries the installed library for Linux x64 and arm64, macOS (universal, 26
or later) and Windows x64 (MSVC, Release and Debug), with a `SHA256SUMS`
file and a build provenance attestation for every archive:

```sh
gh attestation verify dxfio-<version>-linux-x64.tar.gz --repo cadsmith/dxfio
```

[Prebuilt libraries](https://dxfio.dev/guide/start/#prebuilt) says what each
archive was built with.

`Load` and `Save` read and write a drawing's entities and objects on every core
the machine has; `LoadOptions{.Threads = 1}` and `SaveOptions{.Threads = 1}`
keep them to the calling thread, and the drawing, its issues and the bytes come
out the same either way.

It needs CMake 3.28 and a C++23 compiler and standard library, nothing else:
the tests and doctest come in only when dxfio is the top level project. CI
builds it with GCC 15, Clang 23 on libstdc++, Apple Clang on macOS 26 and MSVC
on Windows Server 2025; older toolchains are not tested. It builds as a static
library, or as a shared one with `BUILD_SHARED_LIBS=ON`.

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
`dxfio_bench`, built on request with `--target dxfio_bench`, times load and
save on one drawing read into memory, and `tools/bench/generate.py` makes a
drawing of a given size from a fixture; CI prints the figures of a 10 MB one
in its summary.

## Continuous integration

[ci.yml](.github/workflows/ci.yml) runs on every pull request and push to
`main`:

- clang-format, the CMake guidelines, the public interface's snapshot and the
  pull request's title;
- the `release` preset, built and tested with GCC, Clang, Apple Clang and
  MSVC;
- `sanitize`, `thread`, `memory` and `coverage` with Clang 23;
- `tidy` with clang-tidy 23, on the sources a pull request reaches and on
  every source on a push;
- each fuzz target for a minute, on the corpus the nightly runs grow.

Each job keeps a ccache that a push to `main` saves, so a pull request
compiles only what it changed; one that changes only `docs`, `site` or
Markdown runs the checks alone.
[fuzz.yml](.github/workflows/fuzz.yml) fuzzes each target for twenty minutes
every night and keeps what it finds, and
[codeql.yml](.github/workflows/codeql.yml) runs CodeQL's security and quality
queries, and [release.yml](.github/workflows/release.yml) builds, packs, attests
and publishes the prebuilt libraries for a tag `v<version>`, the version
`CMakeLists.txt` declares; a pull request that changes how they are made
builds and packs them without publishing.

## Layout

| Directory | Holds |
| :--- | :--- |
| `include/dxfio` | The public headers, namespace `DxfIO`; `dxfio/dxfio.hpp` brings in all of them |
| `source` | The implementation and the layout of each kind |
| `tests/dxfio` | Unit tests |
| `tests/api` | The public interface's snapshot, and a check that each header compiles alone |
| `tests/package` | A program that finds the installed library, built by CI against a fresh install |
| `tests/fixtures/acad` | Drawings AutoCAD 2026 wrote for each release |
| `tests/fuzz` | Fuzz targets, their corpus and the inputs they found |
| `tests/bench` | `dxfio_bench`, which times load and save |
| `tools/acad` | The AutoCAD console scripts that made the fixtures, and a verifier |
| `tools/dxf` | The generator of the code page tables |
| `tools/bench` | The generator of large drawings for the bench |
| `tools/format` | `style.py`, which lays out comments and alignment after clang-format |
| `tools/tidy` | The list of sources a change reaches, for clang-tidy |
| `vendor` | Where doctest is fetched from, when `find_package` does not find it |
| `cmake` | The package's config file, installed beside its targets |
| `docs` | The documentation's pages, notes on DXF and on dxfio, and the guidelines |
| `site` | The documentation website that renders `docs`, its examples built and run by `npm run examples` |

## Guidelines

[C++](docs/cpp.md), [commits](docs/commit.md) and [CMake](docs/cmake.md).

## The public interface

Every public declaration has a `/** */` doc block, and every header compiles on
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
