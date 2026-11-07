"""Writes a drawing of a given size for dxfio_bench: a fixture with a realistic mix of entities spliced into
its ENTITIES section, each with a handle of its own, and $HANDSEED moved past them.

    python3 tools/bench/generate.py tests/fixtures/acad/entities-r2018.dxf 100 big.dxf
"""

import random
import sys

KINDS = ("LINE", "LWPOLYLINE", "CIRCLE", "ARC", "TEXT", "POINT", "MTEXT")
OWNER = b"1F"
FIRST_HANDLE = 0x10000


def real(value):
    return repr(round(value, 4)).encode()


def entity(kind, handle, x, y, rng):
    def record(marker, body):
        return [b"  0", kind.encode(), b"  5", format(handle, "X").encode(), b"330", OWNER,
                b"100", b"AcDbEntity", b"  8", b"0", b"100", marker] + body

    if kind == "LINE":
        return record(b"AcDbLine", [b" 10", real(x), b" 20", real(y), b" 30", b"0.0",
                                    b" 11", real(x + 50), b" 21", real(y + 25), b" 31", b"0.0"])
    if kind == "LWPOLYLINE":
        count = rng.randint(4, 12)
        body = [b" 90", str(count).rjust(9).encode(), b" 70", b"     1", b" 43", b"0.0"]
        for _ in range(count):
            body += [b" 10", real(x + rng.uniform(-40, 40)), b" 20", real(y + rng.uniform(-40, 40))]
        return record(b"AcDbPolyline", body)
    if kind == "CIRCLE":
        return record(b"AcDbCircle", [b" 10", real(x), b" 20", real(y), b" 30", b"0.0",
                                      b" 40", real(rng.uniform(1, 30))])
    if kind == "ARC":
        return record(b"AcDbCircle", [b" 10", real(x), b" 20", real(y), b" 30", b"0.0",
                                      b" 40", real(rng.uniform(1, 30)), b"100", b"AcDbArc",
                                      b" 50", real(rng.uniform(0, 180)), b" 51", real(rng.uniform(180, 360))])
    if kind == "TEXT":
        return record(b"AcDbText", [b" 10", real(x), b" 20", real(y), b" 30", b"0.0", b" 40", b"2.5",
                                    b"  1", b"Room %d" % rng.randint(1, 999), b"100", b"AcDbText"])
    if kind == "POINT":
        return record(b"AcDbPoint", [b" 10", real(x), b" 20", real(y), b" 30", b"0.0"])
    return record(b"AcDbMText", [b" 10", real(x), b" 20", real(y), b" 30", b"0.0", b" 40", b"2.5",
                                 b" 41", b"80.0", b" 71", b"     1", b" 72", b"     5",
                                 b"  1", b"{\\fArial|b0|i0;Note %d: \\P the second line}" % rng.randint(1, 999),
                                 b"  7", b"Standard", b" 50", b"0.0"])


def section(lines, name):
    """The index of the group 0 opening the named section."""
    for at in range(len(lines) - 3):
        if lines[at].strip() == b"0" and lines[at + 1] == b"SECTION" and lines[at + 3] == name:
            return at
    raise SystemExit(f"no {name.decode()} section")


def main():
    if len(sys.argv) != 4:
        raise SystemExit(__doc__)
    source, megabytes, target = sys.argv[1], float(sys.argv[2]), sys.argv[3]
    bytes_ = open(source, "rb").read()
    newline = b"\r\n" if b"\r\n" in bytes_[:200] else b"\n"
    lines = bytes_.split(newline)
    begin = section(lines, b"ENTITIES") + 4
    end = begin
    while not (lines[end].strip() == b"0" and lines[end + 1] == b"ENDSEC"):
        end += 1
    rng = random.Random(7)
    wanted = int(megabytes * 1024 * 1024)
    out, size, handle = [], 0, FIRST_HANDLE
    counts = dict.fromkeys(KINDS, 0)
    while size < wanted:
        kind = rng.choice(KINDS)
        counts[kind] += 1
        groups = entity(kind, handle, rng.uniform(0, 10000), rng.uniform(0, 10000), rng)
        handle += 1
        out.extend(groups)
        size += sum(len(group) + len(newline) for group in groups)
    head = lines[:begin]
    for at, line in enumerate(head):
        if line == b"$HANDSEED":
            head[at + 2] = format(handle, "X").encode()
    open(target, "wb").write(newline.join(head + out + lines[end:]))
    print(f"{target}: {sum(counts.values())} entities, {size / 1048576:.1f} MB of them", counts)


if __name__ == "__main__":
    main()
