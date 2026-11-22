# DXF

The `dxfio::dxfio` library reads and writes AutoCAD DXF files, in ASCII and
binary form, for every release from R10 to R2018 (`AC1006` to `AC1032`).

## Use

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

- `Load` reads bytes or a stream and `LoadFile` a file, ASCII or binary, and
  returns the drawing with the issues found while reading: a group given
  twice, a record out of its order, a value that does not parse, a section,
  table or block missing its end. A load fails when the file cannot be read,
  is not DXF, or has a group other than 0 where a record should start; the
  failure says where, for an ASCII file.
- `Save` writes the drawing to bytes or a stream and `SaveFile` to a file,
  for `SaveOptions::Release`, in ASCII or, with `SaveOptions::Binary`, in
  binary. `SaveOptions::Codepage` chooses the code page of releases before
  R2007, which store text in one: ANSI 874 and 1250 to 1258, and the East
  Asian 932, 936, 949, 950 and 1361, of one or two bytes a character. A character the
  page lacks is written as a `\U+` escape; `\M+` escapes are read as the
  character they name. A page not carried keeps its text byte for byte. The
  issues that come back say what the save had to change or leave out.
- `Create(release)` gives an empty drawing AutoCAD opens without a finding:
  the tables, blocks, layouts and objects a drawing of that release needs.
- `Edit::Add` puts an entity in model space, paper space or a block, or a
  record in its table, with a handle where it has none, its owner and
  `$HANDSEED` moved on;
  `Edit::AddBlock` makes a block.
- `FindEntity` and `FindObject` find a record by its handle, and an `Index`
  finds many in constant time. `View` finds table records, blocks, layouts and
  dictionaries by name, without case.
- `TypeOf` and `BaseOf` give any entity's or object's DXF name and its
  handle, owner and extended data.
- `AppearanceOf` gives the layer, color, linetype and lineweight an entity is
  drawn with, its ByLayer and ByBlock resolved; `MethodOf`, `RgbOf`,
  `TransparencyMethodOf` and `AlphaOf` read its color and transparency.
- `Audit::Check` lists what AutoCAD would complain about, and
  `Audit::Repair` fixes it in place; see below.

## What is kept

Reading and writing back the same release keeps every group of every record,
in its order, including what the library has no type for. The bytes can still
differ: a real is written in the shortest form that reads back to the same
value, and lines end in CR LF whatever the file had.

- Each record of a known kind is read into its own type: `DxfIO::Entity` and
  `DxfIO::Object` are variants of them, ending with `Unknown`.
- A record that does not fit its kind's layout, a group out of place or a
  count that does not match, is kept as `Unknown`, whole, and an issue says
  so. Nothing is reordered to make it fit.
- Groups a kind does not name stay with the record they came in, after its
  own fields.
- In an R12 or R10 file, which has no subclass markers to tell one part of a
  record from another, a kind those releases do not have is kept as
  `Unknown`.
- Sections, classes, extended data, extension dictionaries, reactors and
  proxy graphics are kept as read.

## Kinds

Entities typed: 3DFACE, 3DSOLID, ACAD_TABLE, ARC, ARC_DIMENSION, ATTDEF,
ATTRIB, BODY, CIRCLE, DGNUNDERLAY, DIMENSION, DWFUNDERLAY, ELLIPSE,
EXTRUDEDSURFACE, HATCH, HELIX, IMAGE, INSERT, LARGE_RADIAL_DIMENSION, LEADER,
LIGHT, LINE, LOFTEDSURFACE, LWPOLYLINE, MESH, MLINE, MTEXT, MULTILEADER,
NURBSURFACE, OLE2FRAME, PDFUNDERLAY, PLANESURFACE, POINT, POLYLINE, RAY, REGION,
REVOLVEDSURFACE, SECTIONOBJECT, SEQEND, SHAPE, SOLID, SPLINE, SURFACE,
SWEPTSURFACE, TEXT, TOLERANCE, TRACE, VERTEX, VIEWPORT, WIPEOUT, XLINE.

Objects typed: ACDBDATATABLE, ACDBDICTIONARYWDFLT, ACDBPLACEHOLDER, DATATABLE,
DGNDEFINITION, DICTIONARY, DICTIONARYVAR, DWFDEFINITION, GROUP, IMAGEDEF,
IMAGEDEF_REACTOR, LAYOUT, MATERIAL, MLEADERSTYLE, MLINESTYLE, PDFDEFINITION,
PLOTSETTINGS, RASTERVARIABLES, SCALE, SORTENTSTABLE, TABLESTYLE, VISUALSTYLE,
WIPEOUTVARIABLES, XRECORD.

Everything else, CAMERA, FIELD, GEODATA, SUN and the association objects
among them, is kept as `Unknown`: read and written back whole, with no field
of its own to read.

Some kinds keep part of their content as groups rather than fields, because
its meaning depends on the release or is not published: the body of a
surface past its isolines, the properties of a visual style, the overrides of
a mesh and the cells of a data table.

## Saving to another release

A drawing saves to any release; what that release cannot hold is converted
or left out, and the issues the save returns name each entity changed so.

- Kinds a release does not have are left out, each told: before R2000 every
  kind newer than R14, as MESH, MULTILEADER, ACAD_TABLE, LIGHT, HELIX, the
  surfaces, the underlays and ARC_DIMENSION; before R14 HATCH, IMAGE and
  WIPEOUT; before R13 LEADER, TOLERANCE, MLINE, REGION, BODY and 3DSOLID. A
  lightweight polyline becomes a POLYLINE before R14, with no issue.
- For R12 and R10, as AutoCAD does: an ELLIPSE or a SPLINE becomes a 3D
  POLYLINE along its curve, a whole ellipse of 128 segments and an elliptical
  arc of its share of them; a RAY or an XLINE a LINE cut at the drawing's
  extents; an MTEXT a TEXT for each of its lines, its formatting codes
  stripped.
- R12 and R10 keep no objects and no layout but the one drawn in
  `*Paper_Space`; entities keep their handles, table records lose theirs. R10
  keeps no extended data, and neither keeps a layer whose name starts with a
  star.
- A solid, region or surface whose body is in ACDSDATA (R2013 and later)
  cannot be written to an earlier release, nor one whose body is in its own
  lines to a later one: that needs the ACIS modeler's own format. It is left
  out rather than written empty.
- Visual styles change their layout at R2010 and at R2013; saved across
  either, they are left out and what names them is cleared, as a drawing
  AutoCAD writes without them.
- A data table is ACDBDATATABLE in R2000 and DATATABLE after it.
- Objects of a kind dxfio does not type are left out of a save to a later
  release, as their groups are in the form of the one they were read from,
  and so is what they own; each is told, and the dictionary entries,
  reactors and header variables that named them are cleared.
- An R10 or R12 drawing saved to R13 or later is given what the release
  needs: a handle for every record that has none, a BLOCK_RECORD for each
  block, the root dictionary and the layouts, the classes, and the ByLayer and
  ByBlock linetypes. Entities keep their handles; a table record of a name the
  release has, such as layer 0, takes the place of the release's own.

## Audit

`Audit::Check(drawing)` returns a `Report` of findings, each an error
(AutoCAD would refuse or recover the drawing) or a warning (AutoCAD would
open it). `Audit::Repair(drawing)` runs the same checks and fixes what
they find:

- `$HANDSEED` past every handle in use;
- every handle given and none used twice, in the tables dxfio does not type
  too; a duplicate entity takes a new one, and its vertices or attributes
  follow it;
- every table record and block named;
- the records AutoCAD needs: layer 0, the ByBlock, ByLayer and Continuous
  linetypes, the Standard text and dimension styles, the ACAD application and
  the active viewport;
- every application extended data names registered in APPID;
- the layers, linetypes, text styles and blocks entities name; a missing
  layer is made, a missing linetype or style falls back to ByLayer or
  Standard, an insert of a missing block is left out;
- every lineweight one of AutoCAD's 24, else the nearest;
- a mesh with vertices and faces, its edge indices in pairs, one crease per
  edge;
- an ellipse's axis ratio between 0 and 1;
- a leader with two vertices at least;
- a POLYLINE's and an INSERT's SEQEND, and the flags of what follows them;
- a block holding attribute definitions flagged so;
- no group of a code the DXF reference does not define;
- a CLASS for every kind of AutoCAD's own the drawing holds.

## Checked against AutoCAD

The tests read every file under `tests/fixtures/acad`, written by AutoCAD
2026 for each release, and require each to read without an issue, and what it saves
back to read to the same drawing. `tools/acad/*.scr` are the AutoCAD scripts that made them;
`tools/acad/verify.py` opens files in AutoCAD's console and runs its audit.
The `fuzz` preset builds libFuzzer targets that feed broken input to the
reader; the one for whole files saves what it read back to its own release,
which must read back the same, and to R10, R12, R13, R14, R2000, R2007 and
R2018, which must load.

## Known limits

- Saving to R12 leaves out paper space viewports: AutoCAD keeps them, with
  their settings in ACAD MVIEW extended data.
- MTEXT saved to R12 wraps no line to its width: only its own line breaks
  start a new TEXT.
- No type yet for CAMERA, FIELD, GEODATA or SUN: AutoCAD's console makes none
  of them, so there is no sample to type them from.
