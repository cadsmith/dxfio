# How it is checked

How the interface stays stable, what tests stand behind it, and what it does not do yet.

## A stable interface {#interface}

- Everything public is under `include/dxfio`; `dxfio/dxfio.hpp` brings in all of it. The reader's machinery stays in `source`.
- Failures come back in `std::expected`; what was mended or left out, in a list of issues. Nothing throws.
- Every public declaration carries a comment, and every header compiles on its own.
- A snapshot of the public headers is checked in. The `dxfio_api` test fails on any change to it, so a change to the interface is always a deliberate one.

## Tests and AutoCAD {#checked}

- 155 drawings AutoCAD 2026 wrote, for each release, read without an issue, and what dxfio saves back reads to the same document: every group and value, in its order.
- What dxfio writes, to each release, is opened by AutoCAD's console and passes its audit.
- The tests cover every line, function, region and branch; the coverage build fails below that.
- libFuzzer targets feed broken input to the reader, the tags and the text escapes.

## Known limits {#limits}

- No type yet for CAMERA, FIELD, GEODATA or SUN: they are kept whole and written back, with no fields of their own.
- Saving to R12 leaves out paper space viewports; AutoCAD keeps them with ACAD MVIEW extended data.
- MTEXT saved to R12 wraps no line to its width; only its own line breaks start a new TEXT.
