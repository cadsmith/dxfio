#!/usr/bin/env python3
"""AutoCAD DXF Verification Tool.

Audits DXF files using native AutoCAD AcCoreConsole headless CLI.
Verifies that AutoCAD reports 0 errors found and 0 fixed.

One console runs at a time, across processes: they share one isolated
profile. AcCoreConsole sometimes aborts while it starts, before it opens
the drawing (a double free in its setlocale shim); such a run is tried
again rather than taken as the drawing's fault.
"""

import fcntl
import os
import re
import subprocess
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CONSOLE = ROOT / "tools" / "acad" / "console.sh"
LOCK = Path(tempfile.gettempdir()) / "dxfio-acad.lock"
ATTEMPTS = 3

SCRIPT_TEMPLATE = """FILEDIA 0
CMDECHO 0
_.AUDIT _Y
_.QUIT _Y
"""

ERRORS_RE = re.compile(r"Total errors found\s+(\d+)\s+fixed\s+(\d+)")
ERASED_RE = re.compile(r"Erased\s+(\d+)\s+objects")
DISCARDED_RE = re.compile(
    r"(Invalid or incomplete DXF input|drawing discarded)"
)


def run_console(scr_path: str, dxf_path: Path) -> tuple[int, str]:
    """Runs the console alone, again when it dies before it audits."""
    with open(LOCK, "w", encoding="ascii") as lock:
        fcntl.flock(lock, fcntl.LOCK_EX)
        for _ in range(ATTEMPTS):
            try:
                proc = subprocess.run(
                    ["sh", str(CONSOLE), scr_path, str(dxf_path)],
                    capture_output=True,
                    text=True,
                    timeout=60,
                )
            except subprocess.TimeoutExpired:
                continue
            output = proc.stdout + proc.stderr
            if proc.returncode >= 0 or ERRORS_RE.search(output):
                return proc.returncode, output
    return -1, "AutoCAD's console crashed or hung on every attempt"


def audit_dxf(dxf_path: Path) -> tuple[bool, str, int, int, int]:
    """Runs AcCoreConsole audit on the given DXF file."""
    if not CONSOLE.exists():
        return False, "console.sh not found", -1, -1, -1

    with tempfile.NamedTemporaryFile("w", suffix=".scr", delete=False) as f:
        f.write(SCRIPT_TEMPLATE)
        scr_path = f.name

    try:
        returncode, output = run_console(scr_path, dxf_path)

        if DISCARDED_RE.search(output):
            return False, "DXF rejected by AutoCAD", -1, -1, -1

        err_match = ERRORS_RE.search(output)
        era_match = ERASED_RE.search(output)

        if returncode < 0:
            return False, "AutoCAD crashed", -1, -1, -1
        if not err_match or not era_match:
            return False, "Audit summary missing in AutoCAD output", -1, -1, -1

        found = int(err_match.group(1))
        fixed = int(err_match.group(2))
        erased = int(era_match.group(1))

        passed = (
            returncode == 0
            and found == 0
            and fixed == 0
            and erased == 0
        )
        status = "PASSED" if passed else "FAILED"
        return passed, status, found, fixed, erased
    except Exception as exc:  # pylint: disable=broad-except
        return False, f"Exception: {exc}", -1, -1, -1
    finally:
        if os.path.exists(scr_path):
            os.remove(scr_path)


PLACES = (ROOT, Path(tempfile.gettempdir()).resolve())


def placed(given: str) -> Path:
    """The real path of what was given, which must lie in the repository or the temporary directory, where
    the examples the site checks are written."""
    path = Path(given).resolve()
    for place in PLACES:
        if path == place or place in path.parents:
            return path
    raise SystemExit(f"{given} is neither in the repository nor in {PLACES[1]}")


def main() -> int:
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <dxf-file-or-dir>...")
        return 1

    targets: list[Path] = []
    for arg in sys.argv[1:]:
        p = placed(arg)
        if p.is_dir():
            targets.extend(sorted(p.glob("**/*.dxf")))
        elif p.is_file() and p.suffix.lower() == ".dxf":
            targets.append(p)

    if not targets:
        print("No DXF files found.")
        return 1

    print(f"Verifying {len(targets)} DXF file(s) with AutoCAD...")
    print("-" * 75)
    print(f"{'File':<40} | {'Status':<8} | {'Errors':<6} | {'Fixed':<6}")
    print("-" * 75)

    failed_count = 0
    for target in targets:
        passed, status, found, fixed, _ = audit_dxf(target)
        if not passed:
            failed_count += 1
        trunc = target.name[:37] + "..."
        name = target.name if len(target.name) <= 40 else trunc
        err_str = str(found) if found >= 0 else "N/A"
        fix_str = str(fixed) if fixed >= 0 else "N/A"
        print(f"{name:<40} | {status:<8} | {err_str:<6} | {fix_str:<6}")

    print("-" * 75)
    passed_cnt = len(targets) - failed_count
    print(f"Summary: {passed_cnt} passed, {failed_count} failed.")
    return 1 if failed_count > 0 else 0


if __name__ == "__main__":
    sys.exit(main())
