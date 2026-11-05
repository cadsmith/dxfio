# CMake Guidelines

## Rules

- No `#` comments are allowed in any CMake file.
- Lines must not exceed 80 columns.
- The library target is `dxfio`, its alias `dxfio::dxfio`; other targets use
  the `dxfio_` prefix.
- Libraries must be placed in `lib/` and executables in `bin/`.
- All targets must require C++23 (`cxx_std_23`).
- Configuration, building, and testing must use CMake presets.

## Presets

| Name | Type | Purpose |
| :--- | :--- | :--- |
| `debug` | `Debug` | Development with symbols and assertions |
| `release` | `Release` | Optimized build |
| `coverage` | `Debug` at `-O1` | Instrumented build for LLVM coverage |
| `sanitize` | `Debug` at `-O1` | ASan and UBSan instrumented build |
| `tidy` | `Debug` | The library built through clang-tidy |
| `fuzz` | `RelWithDebInfo` | libFuzzer targets, needs LLVM clang on the path |

## Targets

| Target | Alias | Type | Description |
| :--- | :--- | :--- | :--- |
| `dxfio` | `dxfio::dxfio` | Static | DXF format: tags, records, every kind, load and save |
| `dxfio_tests` | - | Executable | Doctest test suite; CTest runs it once per test file, as `dxfio/<file>` |
| `dxfio_headers` | - | Object | Each public header compiled on its own, twice |
| `dxfio_api` | - | Custom, test | Takes the public interface's snapshot; the test compares it |
| `dxfio_fuzz_dxf`, `dxfio_fuzz_tags`, `dxfio_fuzz_escapes` | - | Executable | libFuzzer targets |
| `coverage` | - | Custom | Runs the tests and gates lines, functions, regions and branches at 100% |

## Commands

```sh
cmake --preset debug
cmake --build --preset debug
ctest --preset debug
ctest --preset debug -j 8 -R dxfio/tags/
./build/debug/bin/dxfio_tests -tc="hatch*"

cmake --preset coverage
cmake --build --preset coverage --target coverage

cmake --preset tidy
cmake --build --preset tidy

cmake --preset fuzz
cmake --build --preset fuzz
./build/fuzz/bin/dxfio_fuzz_dxf -dict=tests/fuzz/dxf.dict tests/fuzz/corpus
```
