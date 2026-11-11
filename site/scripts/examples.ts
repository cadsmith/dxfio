// Builds and runs the C++ of every page in docs/guide and docs/tutorials, and the
// home page's example, then checks what each wrote.
//
//   npm run examples                 all of them
//   npm run examples -- layers text  some of them
//   npm run examples -- --acad       and AutoCAD's own AUDIT on every file they wrote
//
// A page's code is the body of a main, or a whole program when it has its own. It passes
// when it compiles with warnings as errors, runs without failing, prints exactly the text
// of its "Output" blocks, and every DXF it writes loads back with no issue and passes
// dxfio's audit (and AutoCAD's, with --acad).
import { execFileSync, spawnSync } from 'node:child_process'
import { copyFileSync, existsSync, mkdirSync, mkdtempSync, readFileSync, readdirSync, writeFileSync } from 'node:fs'
import { tmpdir } from 'node:os'
import { join } from 'node:path'

const site = join(import.meta.dirname, '..')
const root = join(site, '..')
const library = join(root, 'build', 'release', 'lib', 'libdxfio.a')
const compiler = process.env.CXX ?? 'c++'
// The interpreter for AutoCAD's audit, from the fixed places it stands in rather than wherever PATH leads.
const python = ['/usr/bin/python3', '/usr/local/bin/python3', '/opt/homebrew/bin/python3'].find((one) => existsSync(one))
const sample = join(root, 'tests', 'fixtures', 'acad', 'entities-r2018.dxf')
const flags = ['-std=c++23', '-Wall', '-Wextra', '-Werror', `-I${join(root, 'include')}`]

if (!existsSync(library)) {
  console.error(`${library} is missing: cmake --preset release && cmake --build --preset release --target dxfio`)
  process.exit(1)
}

const args = process.argv.slice(2)
const acad = args.includes('--acad')
if (acad && !python) {
  console.log('no python3 in /usr/bin, /usr/local/bin or /opt/homebrew/bin: AutoCAD cannot audit')
  process.exit(1)
}
const wanted = args.filter((one) => !one.startsWith('--'))
const content = join(root, 'docs')
const pages = ['guide', 'tutorials']
  .flatMap((folder) =>
    readdirSync(join(content, folder))
      .filter((name) => name.endsWith('.md'))
      .map((name) => `${folder}/${name.replace(/\.md$/, '')}`),
  )
  .concat('home')
  .filter((page) => wanted.length === 0 || wanted.some((one) => page.endsWith(one)))

// The home page's install tab repeats the example's first line; only the example runs.
const source = (page: string) => {
  const text = readFileSync(join(content, `${page}.md`), 'utf8')
  return page === 'home' ? text.slice(text.indexOf('[#example]')) : text
}

const work = mkdtempSync(join(tmpdir(), 'dxfio-examples-'))

const checker = `#include "dxfio/dxfio.hpp"
#include <print>
auto main(int count, char **paths) -> int
{
  int failed = 0;
  for (int at = 1; at < count; ++at)
  {
    const auto loaded = DxfIO::LoadFile(paths[at]);
    if (!loaded)
    {
      std::println("{}: does not load: line {}: {}", paths[at], loaded.error().Line, loaded.error().What);
      ++failed;
      continue;
    }
    for (const DxfIO::Issue &issue : loaded->Issues)
    {
      std::println("{}: issue on line {}: {}", paths[at], issue.Line, issue.What);
      ++failed;
    }
    const DxfIO::Audit::Report report = DxfIO::Audit::Check(loaded->Document);
    for (const DxfIO::Audit::Finding &finding : report.Findings)
    {
      std::println("{}: audit: {}", paths[at], finding.What);
    }
    if (!report.Clean())
    {
      ++failed;
    }
  }
  return failed == 0 ? 0 : 1;
}
`
writeFileSync(join(work, 'check.cpp'), checker)
execFileSync(compiler, [...flags, join(work, 'check.cpp'), library, '-o', join(work, 'check')], { stdio: 'inherit' })

const program = (body: string) =>
  [
    '#include "dxfio/dxfio.hpp"',
    '',
    '#include <algorithm>',
    '#include <cstdint>',
    '#include <map>',
    '#include <print>',
    '#include <string>',
    '#include <variant>',
    '#include <vector>',
    '',
    'auto main() -> int',
    '{',
    body.replaceAll('#include "dxfio/dxfio.hpp"\n', ''),
    'return 0;',
    '}',
  ].join('\n')

let failures = 0
for (const slug of pages) {
  const text = source(slug)
  const code = [...text.matchAll(/```cpp[^\n]*\n([^]*?)```/g)].map((one) => one[1]).join('\n')
  const expected = [...text.matchAll(/```text title="Output"\n([^]*?)```/g)].map((one) => one[1]).join('')
  const place = join(work, slug)
  mkdirSync(place, { recursive: true })
  copyFileSync(sample, join(place, 'plan.dxf'))
  const built = join(place, 'example')
  writeFileSync(`${built}.cpp`, code.includes('auto main(') ? code : program(code))
  const build = spawnSync(compiler, [...flags, `${built}.cpp`, library, '-o', built], { encoding: 'utf8' })
  if (build.status !== 0) {
    console.log(`✗ ${slug}: does not compile\n${build.stderr}`)
    failures += 1
    continue
  }
  const ran = spawnSync(built, { cwd: place, encoding: 'utf8', timeout: 30000 })
  if (ran.status !== 0) {
    console.log(`✗ ${slug}: exits with ${ran.status}\n${ran.stdout}${ran.stderr}`)
    failures += 1
    continue
  }
  if (expected && ran.stdout !== expected) {
    console.log(`✗ ${slug}: prints something other than its Output block\n--- the page\n${expected}--- the program\n${ran.stdout}`)
    failures += 1
    continue
  }
  const written = readdirSync(place).filter((name) => name.endsWith('.dxf') && name !== 'plan.dxf')
  const checked = spawnSync(join(work, 'check'), written, { cwd: place, encoding: 'utf8' })
  if (checked.status !== 0) {
    console.log(`✗ ${slug}: a file it wrote fails\n${checked.stdout}`)
    failures += 1
    continue
  }
  let autocad = ''
  if (acad) {
    const refused = written.filter((name) => {
      const audit = spawnSync(python ?? '', [join(root, 'tools', 'acad', 'verify.py'), join(place, name)], { encoding: 'utf8', timeout: 120000 })
      if (audit.status !== 0) console.log(`✗ ${slug}: AutoCAD's audit fails ${name}\n${audit.stdout}${audit.stderr}`)
      return audit.status !== 0
    })
    if (refused.length) {
      failures += 1
      continue
    }
    autocad = ', AutoCAD audit clean'
  }
  console.log(`✓ ${slug}: ${written.length ? written.join(', ') : 'no file'} written, read back clean${autocad}`)
}
process.exitCode = failures === 0 ? 0 : 1
