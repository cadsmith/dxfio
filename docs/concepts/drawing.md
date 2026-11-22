---
title: The drawing
---

What a drawing becomes in memory, and what is kept of it.

## Plain values [#model]

A drawing is a `DxfIO::Drawing`, an aggregate of plain values: the header variables, the classes, one member per table, the blocks, the entities and the objects, and any section dxfio does not know, all in the order the file gave them. Copy it, compare it with `==`, change any field.

Each family is a `std::variant` of the kinds dxfio types, ending with `Unknown`: a record of a kind it has no type for keeps its name and every group it came with. Kinds are named after their DXF names: `MText`, `LwPolyline`, `Solid3d`, `VPort`.

## What is kept [#kept]

- Reading and saving back the same release keeps every group of every record, in its order. The bytes can differ: a real is written in the shortest form that reads back to the same value, and lines end in CR LF.
- A record that does not fit its kind, a group out of place or a count that does not match, is kept as `Unknown`, whole, and an issue says so.
- Groups a kind does not name stay with the record they came in, after its own fields.
- Extended data, extension dictionaries, reactors and proxy graphics are kept as read.
- In an R12 or R10 file, a kind those releases do not have is kept as `Unknown`, since they carry no subclass markers to read it by.

## Text and names [#text]

Text in memory is UTF-8, whatever the release stored: a code page's bytes and its `\U+` and `\M+` escapes are read into the characters they stand for, and written back the release's way. A drawing whose `$DWGCODEPAGE` dxfio does not carry is the exception: its text is kept byte for byte, escapes and all, so it saves back the same.

The names records repeat, of a layer, a linetype, a style or a block, are `Symbol`s: each text is stored once for the whole program and shared, so copying one copies a pointer. The store only grows, by one entry for each distinct name a program ever reads. `==` on a `Symbol` compares case and all, while DXF names are the same in any case: `View::Matches` compares them as AutoCAD does, and the `View` lookups use it.

## Pointers into a drawing [#pointers]

`FindEntity`, `FindObject`, an `Index` and the `View` lookups return pointers into the drawing's lists. A pointer is good until the list it points into changes: an `Edit::Add` to that list, an `Audit::Repair`, or any change of your own. Look it up again after one, and build a new `Index`: one holds such pointers for every entity and object.

## Threads [#threads]

`Load` and `Save` read and write the entities and objects of a drawing on every core the machine has; `LoadOptions{.Threads = 1}` and `SaveOptions{.Threads = 1}` keep them to the calling thread, with the same result. Separate drawings can be loaded, edited and saved on separate threads at once, and one drawing read from many threads while none changes it. A drawing being changed is that thread's alone.

## Failures and issues [#issues]

A `Failure` ends the load, the save or the edit; an `Issue` is a note on one that went through. Both carry a `Code` to act on, `What` to show, and the file's `Line` for an ASCII file, none for a binary one or a save.

| `FailureCode` | When |
| :--- | :--- |
| `Io` | A file or stream could not be read or written |
| `Damaged` | The file is not DXF, or its structure is broken past reading |
| `Missing` | An edit needs what the drawing lacks, as a block of that name |
| `Exists` | A name or handle an edit would give is taken |
| `Refused` | An edit or a save the drawing cannot take, as a record with no name |

| `IssueCode` | What happened |
| :--- | :--- |
| `Damaged` | Broken structure the reader stepped over or mended |
| `Unknown` | A value or a kind dxfio does not know, kept or skipped |
| `OutOfRange` | A value the writer could not write as it was, changed or cut |
| `NotKept` | What the release saved to cannot hold, left out or written otherwise |

## Kinds with a type [#kinds]

Every entity and object below has a type of its own, and so do the records of every table. Their fields are in the [reference](/reference/).
