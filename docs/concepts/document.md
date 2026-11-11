---
title: The document
---

What a drawing becomes in memory, and what is kept of it.

## Plain values [#model]

A drawing is a `DxfIO::Document`, an aggregate of plain values: the header variables, the classes, one member per table, the blocks, the entities and the objects, and any section dxfio does not know, all in the order the file gave them. Copy it, compare it with `==`, change any field.

Each family is a `std::variant` of the kinds dxfio types, ending with `Unknown`: a record of a kind it has no type for keeps its name and every group it came with. Kinds are named after their DXF names: `MText`, `LwPolyline`, `Solid3d`, `Vport`.

## What is kept [#kept]

- Reading and saving back the same release keeps every group of every record, in its order. The bytes can differ: a real is written in the shortest form that reads back to the same value, and lines end in CR LF.
- A record that does not fit its kind, a group out of place or a count that does not match, is kept as `Unknown`, whole, and an issue says so.
- Groups a kind does not name stay with the record they came in, after its own fields.
- Extended data, extension dictionaries, reactors and proxy graphics are kept as read.
- In an R12 or R10 file, a kind those releases do not have is kept as `Unknown`, since they carry no subclass markers to read it by.

## Kinds with a type [#kinds]

Every entity and object below has a type of its own, and so do the records of every table. Their fields are in the [reference](/reference/).
