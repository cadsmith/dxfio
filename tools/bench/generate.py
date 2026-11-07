"""Writes a drawing of a given size for dxfio_bench: a fixture with a realistic mix of entities spliced into
its ENTITIES section, each with a handle of its own, and $HANDSEED moved past them. The fixture is read from
the repository and the drawing written under the working directory.

    python3 tools/bench/generate.py tests/fixtures/acad/entities-r2018.dxf 100 big.dxf
"""

import hashlib
import os
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.realpath(__file__))))


def within(base, given):
    """The real path of what was given, which must lie under the base."""
    path = os.path.realpath(given)
    if path != base and not path.startswith(base + os.sep):
        raise SystemExit(f"{given} is not under {base}")
    return path


class Rolls:
    """Numbers drawn from BLAKE2 over a counter: the same drawing every time, and no part of random."""

    def __init__(self, seed):
        self.seed = seed
        self.count = 0

    def fraction(self):
        self.count += 1
        digest = hashlib.blake2b(self.count.to_bytes(8, "little"), digest_size=8, key=self.seed).digest()
        return int.from_bytes(digest, "little") / 2**64

    def uniform(self, low, high):
        return low + (high - low) * self.fraction()

    def randint(self, low, high):
        return low + int(self.fraction() * (high - low + 1))

    def choice(self, items):
        return items[int(self.fraction() * len(items))]

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
    source = within(ROOT, sys.argv[1])
    megabytes = float(sys.argv[2])
    target = within(os.getcwd(), sys.argv[3])
    with open(source, "rb") as file:
        bytes_ = file.read()
    newline = b"\r\n" if b"\r\n" in bytes_[:200] else b"\n"
    lines = bytes_.split(newline)
    begin = section(lines, b"ENTITIES") + 4
    end = begin
    while not (lines[end].strip() == b"0" and lines[end + 1] == b"ENDSEC"):
        end += 1
    rng = Rolls(b"dxfio bench")
    wanted = int(megabytes * 1024 * 1024)
    kept = lines[:begin] + lines[end:]
    out, size, handle = [], sum(len(line) + len(newline) for line in kept), FIRST_HANDLE
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
        if line == b"$HANDSEED" and at > 0 and head[at - 1].strip() == b"9":
            head[at + 2] = format(handle, "X").encode()
            break
    with open(target, "wb") as file:
        file.write(newline.join(head + out + lines[end:]))
    print(f"{target}: {sum(counts.values())} entities, {size / 1048576:.1f} MB", counts)


if __name__ == "__main__":
    main()
