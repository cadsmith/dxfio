# Releases

The ten releases dxfio reads and writes, and what happens to a drawing saved to an older one.

## Every release {#releases}

Every release from R10 to R2018 reads and writes, ASCII or binary. A document saves to any of them; what the release cannot hold is converted, as AutoCAD converts it, or left out, and `Saved::Issues` names each entity changed so.

| Release | `$ACADVER` | Text |
| :--- | :--- | :--- |
| `Version::R10`<span class="cell-note">AutoCAD 1988</span> | `AC1006` | <span class="pill pill-gold">Code page</span> |
| `Version::R12`<span class="cell-note">AutoCAD 1992</span> | `AC1009` | <span class="pill pill-gold">Code page</span> |
| `Version::R13`<span class="cell-note">AutoCAD 1994</span> | `AC1012` | <span class="pill pill-gold">Code page</span> |
| `Version::R14`<span class="cell-note">AutoCAD 1997</span> | `AC1014` | <span class="pill pill-gold">Code page</span> |
| `Version::R2000`<span class="cell-note">AutoCAD 1999</span> | `AC1015` | <span class="pill pill-gold">Code page</span> |
| `Version::R2004`<span class="cell-note">AutoCAD 2003</span> | `AC1018` | <span class="pill pill-gold">Code page</span> |
| `Version::R2007`<span class="cell-note">AutoCAD 2006</span> | `AC1021` | <span class="pill pill-blue">UTF-8</span> |
| `Version::R2010`<span class="cell-note">AutoCAD 2009</span> | `AC1024` | <span class="pill pill-blue">UTF-8</span> |
| `Version::R2013`<span class="cell-note">AutoCAD 2012</span> | `AC1027` | <span class="pill pill-blue">UTF-8</span> |
| `Version::R2018`<span class="cell-note">AutoCAD 2017</span> | `AC1032` | <span class="pill pill-blue">UTF-8</span> |

## Saving to an older one {#older}

**Kinds a release lacks** are left out, and told: no HATCH, IMAGE or WIPEOUT before R14, no 3DSOLID before R13. A lightweight polyline becomes a POLYLINE before R14, with no issue.

**R12 and R10:** an ELLIPSE or SPLINE becomes a 3D POLYLINE along its curve, a whole ellipse of 128 segments and an elliptical arc of its share of them; a RAY or XLINE a LINE cut at the extents; an MTEXT a TEXT for each of its lines. Entities keep their handles, table records lose theirs. No objects, and no layout but the one drawn in `*Paper_Space`; R10 keeps no extended data.

**Solids across R2013:** a body kept in ACDSDATA cannot go to an earlier release, nor one in its own lines to a later one: that needs the ACIS modeler's format. It is left out, never written empty.

> [!CAUTION]
> A 3DSOLID, REGION or surface saved across R2013 is left out of the file. To keep its body, save such a drawing to a release on the same side of R2013 as the one it came from.

**Visual styles** change layout at R2010 and at R2013. Saved across either they are left out, and what names them is cleared.

**Data tables** are ACDBDATATABLE in R2000 and DATATABLE after it.

## Text and code pages {#text}

From R2007 text is UTF-8. Before it, text is in the code page `$DWGCODEPAGE` names: ANSI 874 and 1250 to 1258, and the East Asian 932, 936, 949, 950 and 1361. A character the page lacks is written as a `\U+` escape, and a file whose page is not carried keeps its text as bytes, so it saves back the same.
