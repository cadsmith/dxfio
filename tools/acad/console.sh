#!/bin/sh
set -eu
app_2026="/Applications/Autodesk/AutoCAD 2026/AutoCAD 2026.app"
app=${DXFIO_ACAD:-"$app_2026"}
console="$app/Contents/Helpers/AcCoreConsole.app/Contents/MacOS/AcCoreConsole"
if [ ! -x "$console" ]; then
  app_2027="/Applications/Autodesk/AutoCAD 2027/AutoCAD 2027.app"
  sub="Contents/Helpers/AcCoreConsole.app/Contents/MacOS/AcCoreConsole"
  alt="$app_2027/$sub"
  if [ -x "$alt" ]; then
    console="$alt"
  else
    echo "AcCoreConsole not found at: $console" >&2
    exit 1
  fi
fi

script=$(cd "$(dirname "$1")" && pwd)/$(basename "$1")
user=${TMPDIR:-/tmp}/dxfio-acad
mkdir -p "$user"
if [ $# -gt 1 ]; then
  drawing=$(cd "$(dirname "$2")" && pwd)/$(basename "$2")
  set -- /i "$drawing"
else
  set --
fi
cd "$user"
exec "$console" /isolate dxfio "$user" "$@" /s "$script" /l en-US
