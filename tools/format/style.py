#!/usr/bin/env python3
"""Formats dxfio's C++ sources: clang-format for the code, then every comment as a block.

    python3 -I tools/format/style.py [--check] [file ...]

CLANG_FORMAT names the clang-format to run, LLVM's own by default.

With no file named, every tracked .hpp and .cpp is formatted. With --check, nothing is
written: the files that would change are listed and the exit status is 1.

Comments take two shapes, their text two spaces in, wrapped at the 100th column, and their
tags one table, the tag names, the parameter names and the texts each in a column of its own.

  A file's banner, the block holding @file: Doxygen's visible block (JAVADOC_BANNER), a
  frame of stars from the first column to the hundredth.

      /*************************************************************************************
       *  @file   dxfio/vector.hpp
       *  @brief  Points and directions in space.
       ************************************************************************************/

  Everything else: a JavaDoc block, which clangd shows in a hover with nothing around it,
  where a frame's rows of stars would show as text.

      /**
       *  The unit vector at an angle from the x axis, counterclockwise.
       *
       *  @param  angle The angle, past a turn too.
       *  @return       A vector of length 1.
       */

A /** */ block and a run of whole-line // or /// comments become such a block. A note on a
member or an enumerator is a trailing /**< */, which a trailing ///< becomes: clang-format
aligns where a run of them opens, and the formatter closes them on one column. Where clang-format
has no rule, the formatter lines up three more things: whole numbers in a run of declarations
or enumerators, right-aligned; a braced list of designated struct rows, one a line, its fields
in columns; a run of one-line assertions, on their ==, where their left sides are close; and a
condition broken one operand a line, on its comparisons. A // NOLINT directive stays as it is. clang-format's ReflowComments is
IndentOnly, so the two tools never undo each other.
"""

import os
import pathlib
import re
import shutil
import subprocess
import sys
import textwrap

ROOT = pathlib.Path(__file__).resolve().parents[2]
WIDTH = 100
NAMED = {"@param", "@tparam"}
HEADINGS = {"@def", "@namespace", "@class", "@struct", "@union", "@enum", "@fn", "@var", "@typedef",
            "@defgroup", "@page", "@mainpage"}
CLANG_FORMAT_HINTS = ("/opt/homebrew/opt/llvm/bin/clang-format", "/usr/local/opt/llvm/bin/clang-format")


def clang_format():
    named = os.environ.get("CLANG_FORMAT")
    if named:
        return named
    for hint in CLANG_FORMAT_HINTS:
        if pathlib.Path(hint).exists():
            return hint
    found = shutil.which("clang-format")
    if not found:
        raise SystemExit("clang-format not found")
    return found


def wrap(text, width):
    return textwrap.wrap(text, width=max(width, 20), break_long_words=False, break_on_hyphens=False) or [""]


def paragraphs(body):
    """The body's lines as paragraphs: lists of lines, the empty line between them dropped."""
    out, current = [], []
    for line in body:
        if line.strip():
            current.append(line.strip())
        elif current:
            out.append(current)
            current = []
    if current:
        out.append(current)
    return out


def items(paragraph):
    """A paragraph as items: a tag or a bullet starts one, any other line runs on the one before."""
    out = []
    for line in paragraph:
        if line.startswith(("@", "\\", "- ")) or not out:
            out.append(line)
        else:
            out[-1] += " " + line
    return out


def tag_parts(item):
    words = item.split(None, 2)
    tag = words[0]
    if tag in NAMED and len(words) > 1:
        return tag, words[1], words[2] if len(words) > 2 else ""
    return tag, "", item[len(tag):].strip()


def heading(body):
    """A block's heading command, such as `@def NAME`, which must stay its first line and hold the
    name alone; the text after the name is the block's first paragraph."""
    for index, line in enumerate(body):
        if not line.strip():
            continue
        words = line.split(None, 2)
        if words[0] not in HEADINGS or len(words) < 2:
            return None, body
        rest = [words[2]] if len(words) > 2 else []
        return f"{words[0]} {words[1]}", rest + body[index + 1:]
    return None, body


def render(body, prefix, width):
    """The lines of a block's body, each beginning with the prefix, none past the width."""
    room = width - len(prefix)
    title, body = heading(body)
    prose, tags = [], []
    for paragraph in paragraphs(body):
        if paragraph[0].startswith(("@", "\\")):
            tags.extend(tag_parts(item) for item in items(paragraph))
        else:
            prose.append(items(paragraph))
    lines = []
    for paragraph in prose:
        if lines:
            lines.append("")
        for item in paragraph:
            if item.startswith("- "):
                first, *rest = wrap(item[2:], room - 2)
                lines.append("- " + first)
                lines.extend("  " + more for more in rest)
            else:
                lines.extend(wrap(item, room))
    if tags:
        if lines:
            lines.append("")
        tag_width = max(len(tag) for tag, _, _ in tags)
        name_width = max((len(name) for _, name, _ in tags), default=0)
        column = tag_width + 1 + (name_width + 1 if name_width else 0)
        for tag, name, text in tags:
            head = tag.ljust(tag_width) + " " + (name.ljust(name_width) + " " if name_width else "")
            first, *rest = wrap(text, room - column)
            lines.append((head + first).rstrip())
            lines.extend(" " * column + more for more in rest)
    if title:
        lines.insert(0, title)
    return [(prefix + line).rstrip() for line in lines]


def javadoc(indent, body):
    return [indent + "/**", *render(body, indent + " *  ", WIDTH), indent + " */"]


def banner(indent, body):
    stars = WIDTH - len(indent)
    return [indent + "/" + "*" * (stars - 1), *render(body, indent + " *  ", WIDTH),
            indent + " " + "*" * (stars - 2) + "/"]


def strip_block(lines):
    """The text of a /** */ block's lines, its frame and leading stars taken off."""
    body = []
    for index, line in enumerate(lines):
        text = line.strip()
        if index == 0:
            text = re.sub(r"^/\*+", "", text)
        if index == len(lines) - 1:
            text = re.sub(r"\**/$", "", text)
        text = re.sub(r"^\*+(?=\s|$)", "", text) if index else text
        body.append(text.strip())
    return body


BLOCK_START = re.compile(r"^(\s*)/\*\*")
TRAILING_NOTE = re.compile(r"^(.*\S)\s*///<\s?(.*?)\s*$")
LINE_COMMENT = re.compile(r"^(\s*)//(?!/<)(?!\s*NOLINT)/?\s?(.*)$")


def comments(text):
    lines = text.split("\n")
    out, index = [], 0
    while index < len(lines):
        line = lines[index]
        start = BLOCK_START.match(line)
        if start and not line.strip().startswith("/**<"):
            end = index
            while "*/" not in lines[end] or (end == index and line.strip().endswith("/**")):
                end += 1
            block = lines[index:end + 1]
            indent = start.group(1)
            body = strip_block(block)
            filed = any(line.startswith("@file") for line in body)
            out.extend(banner(indent, body) if filed else javadoc(indent, body))
            index = end + 1
            # The banner stands apart from the include guard after it; a block on a #define stays on it.
            if filed and not indent and index < len(lines) and lines[index].strip().startswith("#"):
                out.append("")
            continue
        comment = LINE_COMMENT.match(line)
        if comment:
            indent = comment.group(1)
            body = []
            while index < len(lines):
                each = LINE_COMMENT.match(lines[index])
                if not each or each.group(1) != indent:
                    break
                body.append(each.group(2))
                index += 1
            out.extend(javadoc(indent, body))
            continue
        note = TRAILING_NOTE.match(line)
        if note:
            line = f"{note.group(1)} /**< {note.group(2)} */"
        out.append(line)
        index += 1
    return "\n".join(out)


ITEM = re.compile(r"(\w*)\{((?:\.\w+ = [^{},]+)(?:, \.\w+ = [^{},]+)*)\}")
FIELD = re.compile(r"\.(\w+) = (.+)")
NUMBER = re.compile(r"^[-+]?(?:0x[0-9A-Fa-f.'p+-]+|[0-9][0-9'.]*(?:e[-+]?[0-9]+)?)[fFlLuU]*$")


def row_items(line):
    """The struct rows a line holds, when it holds nothing else: `Name{.A = 1, .B = 2},` each."""
    rest = line.strip()
    found = []
    while rest:
        item = ITEM.match(rest)
        if not item:
            return None
        found.append((item.group(1), [FIELD.match(field).groups() for field in item.group(2).split(", ")]))
        rest = rest[item.end():]
        comma = rest.startswith(",")
        rest = rest[1:].lstrip() if comma else rest
        if not comma and rest:
            return None
        found[-1] += (comma,)
    return found or None


def tables(text):
    """A braced list of designated struct rows, one row a line, each field a column: numbers
    right-aligned, anything else left-aligned. clang-format packs such a list into as many
    rows a line as fit, and has no rule against it."""
    lines = text.split("\n")
    out = []
    index = 0
    while index < len(lines):
        run = []
        while index + len(run) < len(lines) and row_items(lines[index + len(run)]):
            run.append(lines[index + len(run)])
        if not run:
            out.append(lines[index])
            index += 1
            continue
        indent = re.match(r"^\s*", run[0]).group(0)
        rows = [row for line in run for row in row_items(line)]
        shapes = {(name, tuple(field for field, _ in fields)) for name, fields, _ in rows}
        if len(shapes) != 1:
            out.extend(run)
            index += len(run)
            continue
        columns = list(zip(*[[value for _, value in fields] for _, fields, _ in rows]))
        widths = [max(len(value) for value in column) for column in columns]
        numeric = [all(NUMBER.match(value) for value in column) for column in columns]
        for name, fields, comma in rows:
            cells = []
            for column, (field, value) in enumerate(fields):
                shown = value.rjust(widths[column]) if numeric[column] else value.ljust(widths[column])
                cells.append(f".{field} = {shown}")
            out.append(f"{indent}{name}{{{', '.join(cells)}}}{',' if comma else ''}")
        index += len(run)
    return "\n".join(out)


CHECK = re.compile(r"^(\s*)(CHECK|CHECK_FALSE|REQUIRE|REQUIRE_FALSE|static_assert)\((.*)\);$")
SPREAD = 12


def equality(inside):
    """Where an assertion's one == stands, outside every bracket; none for none or several."""
    depth, found = 0, []
    for index, char in enumerate(inside):
        if char in "([{":
            depth += 1
        elif char in ")]}":
            depth -= 1
        elif depth == 0 and inside.startswith(" == ", index):
            found.append(index)
    return found[0] if len(found) == 1 else None


def align(lines, first, group):
    """One group of assertions lined up on their ==, if it holds two or more and fits the width."""
    if len(group) < 2:
        return
    lefts = [len(check.group(1)) + len(check.group(2)) + 1 + equality(check.group(3)) for check in group]
    column = max(lefts)
    if any(len(lines[first + offset]) + column - left > WIDTH for offset, left in enumerate(lefts)):
        return
    for offset, check in enumerate(group):
        indent, macro, inside = check.groups()
        at = equality(inside)
        lines[first + offset] = f"{indent}{macro}({inside[:at].ljust(column - len(indent) - len(macro) - 1)}{inside[at:]});"


def checks(text):
    """A run of one-line assertions, each with one ==, lined up on it, so the expected values
    read as a column. The run is cut into groups whose left sides differ by no more than a few
    characters, so that no line takes a long gap, and each group lined up on its own."""
    lines = text.split("\n")
    index = 0
    while index < len(lines):
        run = []
        while index + len(run) < len(lines):
            check = CHECK.match(lines[index + len(run)])
            if not check or equality(check.group(3)) is None:
                break
            run.append(check)
        lefts = [len(check.group(1)) + len(check.group(2)) + 1 + equality(check.group(3)) for check in run]
        start = 0
        while start < len(run):
            end = start + 1
            while end < len(run) and max(lefts[start:end + 1]) - min(lefts[start:end + 1]) <= SPREAD:
                end += 1
            align(lines, index + start, run[start:end])
            start = end
        index += len(run) or 1
    return "\n".join(lines)


LINK = re.compile(r"^(\s*)(&&|\|\|) ")
COMPARISONS = ("<=", ">=", "==", "!=", "<", ">")


def comparison(operand):
    """Where an operand's one comparison stands, outside every bracket, and how long it is;
    none for none or several."""
    depth, found, index = 0, [], 0
    while index < len(operand):
        char = operand[index]
        if char in "([{":
            depth += 1
        elif char in ")]}":
            depth -= 1
        elif depth == 0 and operand[index - 1:index] == " ":
            for operator in COMPARISONS:
                if operand.startswith(operator + " ", index):
                    found.append((index, len(operator)))
                    index += len(operator)
                    break
        index += 1
    return found[0] if len(found) == 1 else None


def chains(text):
    """A condition broken one operand a line, each operand one comparison, lined up on its
    comparisons, so that what is compared reads as two columns:

        return foo.Min.Baz <= bar.Baz
            && bar.Baz     <= foo.Max.Baz;
    """
    lines = text.split("\n")
    index = 0
    while index + 1 < len(lines):
        link = LINK.match(lines[index + 1])
        if not link or LINK.match(lines[index]):
            index += 1
            continue
        start = link.end()
        run = [index]
        while run[-1] + 1 < len(lines) and LINK.match(lines[run[-1] + 1]) and LINK.match(lines[run[-1] + 1]).end() == start:
            run.append(run[-1] + 1)
        operands = [lines[line][start:] for line in run]
        found = [comparison(operand) for operand in operands]
        if len(lines[index]) > start and all(found):
            column = max(len(operand[:at].rstrip()) for operand, (at, _) in zip(operands, found))
            aligned = [operand[:at].rstrip().ljust(column) + " " + operand[at:].lstrip()
                       for operand, (at, _) in zip(operands, found)]
            if all(len(lines[line][:start]) + len(text) <= WIDTH for line, text in zip(run, aligned)):
                for line, operand in zip(run, aligned):
                    lines[line] = lines[line][:start] + operand
        index = run[-1] + 1
    return "\n".join(lines)


VALUE = re.compile(
    r"^(\s*(?![*/])[^=;(){}]*?\w\s*=\s)([-+]?(?:0x[0-9A-Fa-f']+|[0-9][0-9']*)[uUlL]*)([;,]?)(\s*)(/\*\*<.*)?$")


def values(text):
    """Whole numbers in a run of declarations or enumerators, right-aligned, so that digits of
    a place line up and a sign stands out to the left of them rather than pushing them: `= -1`,
    `=  0`. The padding comes out of the space before a trailing note, so the notes keep their
    column. A run holding anything but whole numbers is left as it is."""
    lines = text.split("\n")
    index = 0
    while index < len(lines):
        run = []
        while index + len(run) < len(lines) and VALUE.match(lines[index + len(run)]):
            run.append(VALUE.match(lines[index + len(run)]))
        width = max((len(value.group(2)) for value in run), default=0)
        for offset, value in enumerate(run):
            head, number, comma, gap, note = value.groups()
            pad = width - len(number)
            if note:
                gap = " " * max(len(gap) - pad, 1)
            lines[index + offset] = f"{head}{number.rjust(width)}{comma}{gap}{note or ''}".rstrip()
        index += len(run) or 1
    return "\n".join(lines)


NOTE = re.compile(r"^(.*\S)(\s+)/\*\*<\s(.*?)\s*\*/\s*$")


def notes(text):
    """Trailing /**< */ notes on consecutive lines, closed on one column: each note's text
    padded to the longest of its run. clang-format aligns where they open, and leaves the
    spaces inside a comment as they are."""
    lines = text.split("\n")
    index = 0
    while index < len(lines):
        run = []
        while index + len(run) < len(lines) and NOTE.match(lines[index + len(run)]):
            run.append(NOTE.match(lines[index + len(run)]))
        width = max((len(note.group(3)) for note in run), default=0)
        for offset, note in enumerate(run):
            code, gap, words = note.groups()
            lines[index + offset] = f"{code}{gap}/**< {words.ljust(width)} */"
        index += len(run) or 1
    return "\n".join(lines)


def sources(named):
    if named:
        return [pathlib.Path(name).resolve() for name in named]
    listed = subprocess.run(["git", "ls-files", "--cached", "--others", "--exclude-standard",
                             "*.hpp", "*.cpp"], cwd=ROOT, check=True, capture_output=True,
                            text=True).stdout.split()
    return [ROOT / name for name in listed]


def formatted(path, tool):
    code = subprocess.run([tool, f"--assume-filename={path}"], input=path.read_text(),
                          check=True, capture_output=True, text=True, cwd=ROOT).stdout
    once = comments(code)
    twice = subprocess.run([tool, f"--assume-filename={path}"], input=once, check=True,
                           capture_output=True, text=True, cwd=ROOT).stdout
    return notes(values(chains(checks(tables(twice)))))


def main():
    arguments = sys.argv[1:]
    check = "--check" in arguments
    named = [argument for argument in arguments if argument != "--check"]
    tool = clang_format()
    changed = []
    for path in sources(named):
        before = path.read_text()
        after = formatted(path, tool)
        if after != before:
            changed.append(path.relative_to(ROOT))
            if not check:
                path.write_text(after)
    for path in changed:
        print(("would change: " if check else "formatted: ") + str(path))
    sys.exit(1 if check and changed else 0)


if __name__ == "__main__":
    main()
