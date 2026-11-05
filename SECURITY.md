# Security Policy

## Reporting a vulnerability

Report it through GitHub's private vulnerability reporting:
<https://github.com/cadsmith/dxfio/security/advisories/new>. The report is
visible to the maintainers alone until an advisory is published. Do not open
an issue or a pull request for it; both are public.

Include the DXF file that triggers the problem, or the smallest file that
still does; the dxfio release, compiler and platform; and the output as it
was printed: the sanitizer report, the stack trace or the fuzzer's artifact.
The fuzz targets under `tests/fuzz` take a file on the command line, so a
fuzzer finding can be sent as it is.

You get a first response within 14 days. Please allow 90 days from the
report before disclosing it publicly.

## What counts

A security vulnerability is a crash, a read or write outside bounds, a read
of uninitialised memory, undefined behaviour, a hang or unbounded memory use
that a malformed file causes through `DxfIO::Load`, `DxfIO::LoadFile`,
`DxfIO::Save` or `DxfIO::SaveFile`.

It is not a vulnerability, and belongs in a public issue, when the calling
program misuses the API; when AutoCAD or another program reads a drawing
dxfio wrote wrongly; or when it affects the documentation site or the build
and test tooling.

## Supported versions

dxfio is before 1.0. Fixes land on `main` and ship in the next release, and
only the latest release receives them.

## Disclosure and credit

Once a fix is released, the advisory is published and the release notes name
the vulnerability. Both credit the reporter unless they ask not to be named.
The fix comes with a test that pins the input, and the input joins
`tests/fuzz/found`, which the test suite replays.

## Reports from tools

A report from a static analyser, a fuzzer or an AI tool goes through the
same private channel, and is most useful with a file that reproduces the
problem. Triage asks for one when it is missing, and a report that nothing
reproduces is closed, still in private.
