---
title: Releases
---

The ten releases dxfio reads and writes, and what happens to a drawing saved to an older one.

## Every release [#releases]

Every release from R10 to R2018 reads and writes, ASCII or binary. A document saves to any of them; what the release cannot hold is converted, as AutoCAD converts it, or left out, and `Saved::Issues` names each entity changed so.

| Release | AutoCAD | `$ACADVER` | Text |
| :--- | :--- | :--- | :--- |
| `Version::R10` | AutoCAD 1988 | `AC1006` | Code page |
| `Version::R12` | AutoCAD 1992 | `AC1009` | Code page |
| `Version::R13` | AutoCAD 1994 | `AC1012` | Code page |
| `Version::R14` | AutoCAD 1997 | `AC1014` | Code page |
| `Version::R2000` | AutoCAD 1999 | `AC1015` | Code page |
| `Version::R2004` | AutoCAD 2003 | `AC1018` | Code page |
| `Version::R2007` | AutoCAD 2006 | `AC1021` | UTF-8 |
| `Version::R2010` | AutoCAD 2009 | `AC1024` | UTF-8 |
| `Version::R2013` | AutoCAD 2012 | `AC1027` | UTF-8 |
| `Version::R2018` | AutoCAD 2017 | `AC1032` | UTF-8 |

## Saving to an older one [#older]

**Kinds a release lacks** are left out, and told: no HATCH, IMAGE or WIPEOUT before R14, no 3DSOLID before R13. A lightweight polyline becomes a POLYLINE before R14, with no issue.

**R12 and R10:** an ELLIPSE or SPLINE becomes a 3D POLYLINE along its curve, a whole ellipse of 128 segments and an elliptical arc of its share of them; a RAY or XLINE a LINE cut at the extents; an MTEXT a TEXT for each of its lines. Entities keep their handles, table records lose theirs. No objects, and no layout but the one drawn in `*Paper_Space`; R10 keeps no extended data.

**Solids across R2013:** a body kept in ACDSDATA cannot go to an earlier release, nor one in its own lines to a later one: that needs the ACIS modeler's format. It is left out, never written empty.

> [!CAUTION]
> A 3DSOLID, REGION or surface saved across R2013 is left out of the file. To keep its body, save such a drawing to a release on the same side of R2013 as the one it came from.

**Visual styles** change layout at R2010 and at R2013. Saved across either they are left out, and what names them is cleared.

**Data tables** are ACDBDATATABLE in R2000 and DATATABLE after it.

## Text and code pages [#text]

From R2007 text is UTF-8. Before it, text is in the code page `$DWGCODEPAGE` names: ANSI 874 and 1250 to 1258, and the East Asian 932, 936, 949, 950 and 1361. A character the page lacks is written as a `\U+` escape, and a file whose page is not carried keeps its text as bytes, so it saves back the same.
