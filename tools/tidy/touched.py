"""Prints the library sources a change reaches, for clang-tidy to check them alone.

    python3 tools/tidy/touched.py build/tidy/compile_commands.json [base]

With a base ref, the changed files are those of `git diff` from the merge base to HEAD: a changed source is
printed, and so is every source whose preprocessed dependencies, from the compiler's -MM, hold a changed
header. A change to the tidy configuration, a CMake file or this tool prints every source, as no base does.
"""

import concurrent.futures
import json
import os
import pathlib
import shlex
import subprocess
import sys

EVERYTHING = (".clang-tidy", "CMakePresets.json", ".github/workflows/ci.yml")


def changed(root, base):
    merge_base = subprocess.run(["git", "merge-base", base, "HEAD"], cwd=root, check=True,
                                capture_output=True, text=True).stdout.strip()
    diff = subprocess.run(["git", "diff", "--name-only", merge_base, "HEAD"], cwd=root, check=True,
                          capture_output=True, text=True).stdout
    return {line for line in diff.splitlines() if line}


def whole(touched):
    """Whether the change reaches every source: the tidy configuration, a CMake file or this tool."""
    return any(path in EVERYTHING or path.startswith("tools/tidy/") or path.endswith("CMakeLists.txt")
               for path in touched)


def sources(commands, root):
    """The compile commands of the library's sources, by their path relative to the root."""
    out = {}
    for entry in commands:
        file = pathlib.Path(entry["directory"], entry["file"]).resolve()
        if not file.is_relative_to(root):
            continue
        relative = file.relative_to(root)
        if relative.parts[0] == "source" and relative.suffix == ".cpp":
            out[str(relative)] = entry
    return out


def dependencies(entry, root):
    """The files the source includes, relative to the root, from the compiler's -MM."""
    arguments = entry.get("arguments") or shlex.split(entry["command"])
    stripped = []
    skip = False
    for argument in arguments:
        if skip:
            skip = False
        elif argument == "-o":
            skip = True
        elif argument != "-c":
            stripped.append(argument)
    listed = subprocess.run(stripped + ["-MM"], cwd=entry["directory"], check=True,
                            capture_output=True, text=True).stdout
    out = set()
    for token in listed.replace("\\\n", " ").split()[1:]:
        path = pathlib.Path(entry["directory"], token).resolve()
        if path.is_relative_to(root):
            out.add(str(path.relative_to(root)))
    return out


def main():
    if len(sys.argv) not in (2, 3):
        raise SystemExit(__doc__)
    with open(sys.argv[1], encoding="utf-8") as file:
        commands = json.load(file)
    root = pathlib.Path(__file__).resolve().parents[2]
    library = sources(commands, root)
    touched = changed(root, sys.argv[2]) if len(sys.argv) == 3 else None
    if touched is None or whole(touched):
        print("\n".join(sorted(library)))
        return
    reached = {path for path in library if path in touched}
    rest = [path for path in library if path not in reached]
    with concurrent.futures.ThreadPoolExecutor(max_workers=os.cpu_count()) as pool:
        for path, deps in zip(rest, pool.map(lambda p: dependencies(library[p], root), rest)):
            if deps & touched:
                reached.add(path)
    print("\n".join(sorted(reached)))


if __name__ == "__main__":
    main()
