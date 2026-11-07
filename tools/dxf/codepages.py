#!/usr/bin/env python3
"""Generate the code page tables dxf strings use before R2007.

usage: codepages.py OUTPUT.hpp

Bytes below 128 are ascii in every page; the upper half comes from Python's
codecs, with a byte the page leaves undefined mapped to itself, as Windows
does. The East Asian pages add a table of their two byte characters: a lead
byte from 0x81 to 0xFE, then a trail byte from 0x30 to 0xFE, 0 where the
pair is none.
"""

import codecs
import os
import sys

SINGLE = ["874"] + [str(n) for n in range(1250, 1259)]

DOUBLE = {"932": "cp932", "936": "gbk", "949": "cp949", "950": "cp950",
          "1361": "johab"}

LEADS = range(0x81, 0xFF)

TRAILS = range(0x30, 0xFF)


def pairs(codec):
    out = []
    for lead in LEADS:
        for trail in TRAILS:
            try:
                text = bytes([lead, trail]).decode(codec)
                out.append(ord(text) if len(text) == 1 else 0)
            except UnicodeDecodeError:
                out.append(0)
    return out


def upper_half(codec):
    out = []
    for byte in range(128, 256):
        try:
            text = codecs.decode(bytes([byte]), codec)
            out.append(ord(text) if len(text) == 1 else byte)
        except UnicodeDecodeError:
            out.append(byte)
    return out


def table(values):
    rows = [", ".join(f"0x{v:04x}" for v in values[i:i + 8])
            for i in range(0, len(values), 8)]
    return ",\n    ".join(rows)


ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.realpath(__file__))))


def within(base, given):
    """The real path of what was given, which must lie under the base: the header is the repository's."""
    path = os.path.realpath(given)
    if path != base and not path.startswith(base + os.sep):
        raise SystemExit(f"{given} is not under {base}")
    return path


def main(argv):
    if len(argv) != 2:
        print(__doc__)
        return 2
    names = SINGLE + list(DOUBLE)
    lines = ["/* generated from python's codecs by tools/dxf/codepages.py, "
             "do not edit */",
             "#ifndef DXFIO_ENCODING_PAGES_HPP",
             "#define DXFIO_ENCODING_PAGES_HPP", "",
             "#include <array>", "#include <cstddef>", "#include <span>",
             "#include <string_view>", "",
             "namespace DxfIO::Generated", "{",
             f"inline constexpr std::size_t Leads = {len(LEADS)};", "",
             f"inline constexpr std::size_t Trails = {len(TRAILS)};", "",
             f"inline constexpr unsigned char Lead = 0x{LEADS.start:02x};", "",
             f"inline constexpr unsigned char Trail = 0x{TRAILS.start:02x};",
             ""]
    for page, codec in DOUBLE.items():
        lines.append(f"inline constexpr std::array<char16_t, Leads * Trails> "
                     f"Pairs{page} = {{")
        lines.append("    " + table(pairs(codec)) + "};")
        lines.append("")
    lines += ["struct Page", "{", "  std::string_view          Name;",
              "  std::array<char16_t, 128> High;",
              "  std::span<const char16_t> Pairs;", "};", "",
              f"inline constexpr std::array<Page, {len(names)}> Pages = {{{{"]
    for page in names:
        codec = DOUBLE.get(page, "cp" + page)
        pairs_name = f"Pairs{page}" if page in DOUBLE else "{}"
        lines.append(f'    {{"ANSI_{page}",')
        lines.append("     {{" + table(upper_half(codec)) + "}},")
        lines.append(f"     {pairs_name}}},")
    lines += ["}};", "", "} // namespace DxfIO::Generated", "",
              "#endif /* DXFIO_ENCODING_PAGES_HPP */", ""]
    with open(within(ROOT, argv[1]), "w", encoding="ascii") as out:
        out.write("\n".join(lines))
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
