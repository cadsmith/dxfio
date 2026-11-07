import { readFileSync, readdirSync, statSync, writeFileSync } from 'node:fs'
import { join, relative } from 'node:path'
import { paint } from './highlight.ts'

type Field = { type: string; name: string; value?: string; doc: string }
type Method = { signature: string; doc: string; html?: string }
type Struct = {
  kind: 'struct' | 'class'
  name: string
  template?: string
  templateHtml?: string
  doc: string
  fields: Field[]
  methods: Method[]
}
type Value = { name: string; doc: string }
type Enum = { name: string; doc: string; values: Value[] }
type Alias = { name: string; target: string; doc: string; html?: string }
type Constant = { type: string; name: string; value: string; doc: string }
type Fn = { name: string; signature: string; doc: string; html?: string }
type Header = {
  path: string
  include?: string
  namespace: string
  group: string
  structs: Struct[]
  enums: Enum[]
  aliases: Alias[]
  constants: Constant[]
  functions: Fn[]
}

const root = join(import.meta.dirname, '..', '..')
const include = join(root, 'include', 'dxfio')
const source = join(root, 'source')

const walk = (dir: string): string[] =>
  readdirSync(dir).flatMap((name) => {
    const path = join(dir, name)
    return statSync(path).isDirectory() ? walk(path) : [path]
  })

const squeeze = (text: string) => text.replace(/\s+/g, ' ').trim()

const groups: [string, (path: string) => boolean][] = [
  ['Start', (p) => ['dxfio.hpp', 'load.hpp', 'save.hpp', 'create.hpp', 'version.hpp'].includes(p)],
  ['Edit', (p) => p.startsWith('edit/')],
  ['Document', (p) => ['document.hpp', 'section.hpp', 'issue.hpp', 'failure.hpp', 'unknown.hpp', 'raw.hpp'].includes(p) || p.startsWith('header/')],
  ['Entities', (p) => p.startsWith('entities/') || p === 'entity.hpp'],
  ['Objects', (p) => p.startsWith('objects/') || p === 'object.hpp'],
  ['Tables', (p) => p.startsWith('tables/')],
  ['Blocks', (p) => p.startsWith('blocks/')],
  ['Audit', (p) => p.startsWith('audit/')],
  ['Values', () => true],
]

const docBefore = (text: string, at: number): string => {
  const lines = text.slice(0, text.lastIndexOf('\n', at - 1) + 1).split('\n')
  lines.pop()
  const out: string[] = []
  while (lines.length > 0) {
    const line = lines.pop()?.trim() ?? ''
    if (!line.startsWith('///')) break
    out.unshift(line.replace(/^\/\/\/\s?/, ''))
  }
  return out.join(' ')
}

const closing = (text: string, open: number): number => {
  let depth = 0
  let at = open
  while (at < text.length) {
    if (text[at] === '/' && text[at + 1] === '/') {
      const end = text.indexOf('\n', at)
      if (end < 0) return text.length
      at = end + 1
      continue
    }
    if (text[at] === '{') depth += 1
    if (text[at] === '}') {
      depth -= 1
      if (depth === 0) return at
    }
    at += 1
  }
  return text.length
}

// The code without a body in braces at its end, and without the semicolon after one.
const bodiless = (code: string): string => {
  const brace = code.indexOf('{')
  if (brace < 0 || !/\}\s*;?$/.test(code)) return code
  return code.slice(0, brace).trimEnd()
}

// A constructor's signature without its initializer list: what follows the closing parenthesis and a lone colon.
const uninitialized = (code: string): string => {
  const colon = code.search(/\)\s*:(?!:)/)
  return colon < 0 ? code : `${code.slice(0, colon)})`
}

// A field as `type name`, `type name = value` or `type name{value}`: the name is the last word before the value.
const field = (code: string): { type: string; name: string; value?: string } | undefined => {
  const whole = code.endsWith(';') ? code.slice(0, -1) : undefined
  if (whole === undefined) return undefined
  const equals = whole.indexOf('=')
  const brace = whole.indexOf('{')
  let head = whole
  let value: string | undefined
  if (equals >= 0 && (brace < 0 || equals < brace)) {
    head = whole.slice(0, equals)
    value = whole.slice(equals + 1).trimStart()
  } else if (brace >= 0 && whole.endsWith('}')) {
    head = whole.slice(0, brace)
    value = whole.slice(brace + 1, -1)
  }
  head = head.trimEnd()
  const space = head.search(/\s\w+$/)
  if (space < 0) return undefined
  return { type: head.slice(0, space), name: head.slice(space + 1), value }
}

type Statement = { code: string; doc: string }

const statements = (body: string): Statement[] => {
  const out: Statement[] = []
  let depth = 0
  let code = ''
  let doc: string[] = []
  const flush = () => {
    if (squeeze(code)) out.push({ code: squeeze(code), doc: doc.join(' ') })
    code = ''
    doc = []
  }
  for (const raw of body.split('\n')) {
    const line = raw.trim()
    if (depth === 0 && line.startsWith('///')) {
      doc.push(line.replace(/^\/\/\/\s?/, ''))
      continue
    }
    if (line.startsWith('//')) continue
    if (depth === 0 && /^(public|private|protected):$/.test(line)) {
      code = line
      flush()
      continue
    }
    const opened = depth
    for (const char of raw) {
      if (char === '{' || char === '(') depth += 1
      if (char === '}' || char === ')') depth -= 1
      code += char
      if (char === ';' && depth === 0) flush()
    }
    code += '\n'
    if (depth === 0 && opened === 0 && line.endsWith('}')) flush()
    if (depth === 0 && opened > 0 && line === '}') flush()
  }
  return out
}

const membersOf = (body: string, kind: 'struct' | 'class') => {
  const fields: Field[] = []
  const methods: Method[] = []
  let visible = kind === 'struct'
  for (const one of statements(body)) {
    if (one.code === 'public:') visible = true
    if (one.code === 'private:' || one.code === 'protected:') visible = false
    if (!visible || one.code.includes('operator') || /^(using|static|friend|template)\b/.test(one.code)) continue
    const stripped = bodiless(one.code)
    if ((/^[\w:<>, ]*\(/.test(stripped) || /\w\s*\(/.test(stripped)) && !/^[^(]*=/.test(one.code)) {
      const signature = uninitialized(stripped.replace(/;$/, '').replace(/^\[\[nodiscard\]\]\s*/, ''))
      methods.push({ signature: squeeze(signature), doc: one.doc })
      continue
    }
    const parsed = field(one.code)
    if (!parsed) continue
    fields.push({ ...parsed, doc: one.doc })
  }
  return { fields, methods }
}

const names = new Map<string, string>()
for (const path of walk(source).filter((p) => p.endsWith('.hpp'))) {
  for (const match of readFileSync(path, 'utf8').matchAll(/Type<([\w:]+)>\s*=\s*"([^"]+)"/g)) {
    names.set(match[1], match[2])
  }
}

const headers: Header[] = walk(include)
  .filter((path) => path.endsWith('.hpp'))
  .sort((one, other) => (one < other ? -1 : one > other ? 1 : 0))
  .map((path) => {
    const text = readFileSync(path, 'utf8')
    const rel = relative(include, path)
    const namespace = /namespace\s+(DxfIO(?:::\w+)*)\s*\{/.exec(text)?.[1] ?? 'DxfIO'
    const structs: Struct[] = []
    const spans: [number, number][] = []
    const pattern = /(template\s*<[^>]*>\s*)?(?<!enum )\b(struct|class)\s+(?:DXFIO_API\s+)?(\w+)\s*(?::[^{;]*)?\{/g
    for (const match of text.matchAll(pattern)) {
      const at = match.index ?? 0
      const line = text.slice(text.lastIndexOf('\n', at) + 1, at)
      if (line.trim().startsWith('//')) continue
      const open = at + match[0].length - 1
      const end = closing(text, open)
      spans.push([at, end])
      if (['std', 'hash', 'formatter'].includes(match[3])) continue
      const kind = match[2] as 'struct' | 'class'
      structs.push({
        kind,
        name: match[3],
        template: match[1] ? squeeze(match[1]) : undefined,
        doc: docBefore(text, at),
        ...membersOf(text.slice(open + 1, end), kind),
      })
    }
    const outside = (at: number) => !spans.some(([from, to]) => at > from && at < to)
    const enums = [...text.matchAll(/enum class (\w+)\s*:\s*[\w:]+\s*\{([^}]*)\}/g)].map((one) => ({
      name: one[1],
      doc: docBefore(text, one.index ?? 0),
      values: statements(one[2].replace(/,\s*$/gm, ';'))
        .map((value) => {
          const code = value.code.replace(/;$/, '')
          const equals = code.indexOf('=')
          return { name: (equals < 0 ? code : code.slice(0, equals)).trimEnd(), doc: value.doc }
        })
        .filter((value) => /^\w+$/.test(value.name)),
    }))
    const aliases = [...text.matchAll(/^[ \t]*using (\w+) =([^;]+);/gm)]
      .filter((one) => outside(one.index ?? 0))
      .map((one) => ({ name: one[1], target: squeeze(one[2]), doc: docBefore(text, (one.index ?? 0) + one[0].indexOf('using')) }))
    const constants = [...text.matchAll(/^[ \t]*inline constexpr ([^=;{\n]+) (\w+)(?: = ([^;]+)|\{([^;]*)\});/gm)]
      .filter((one) => outside(one.index ?? 0))
      .map((one) => ({
        type: squeeze(one[1]),
        name: one[2],
        value: squeeze(one[3] ?? one[4] ?? ''),
        doc: docBefore(text, (one.index ?? 0) + one[0].indexOf('inline')),
      }))
    const declared = /^[ \t]*(?:\[\[nodiscard\]\] )?DXFIO_API (auto|void) (\w+)\(([^;]*)\)(\s*->[^;{]+)?;/gm
    const templated = /^(template <[^>]*>\n(?:[ \t]*requires[^\n]*\n)?)[ \t]*\[\[nodiscard\]\] auto (\w+)\(([^)]*)\)(\s*->[^{;]+)\{/gm
    const functions = [
      ...[...text.matchAll(declared)].map((one) => ({ one, head: '' })),
      ...[...text.matchAll(templated)].map((one) => ({ one, head: squeeze(one[1]) + ' ' })),
    ]
      .filter(({ one }) => outside(one.index ?? 0))
      .sort((a, b) => (a.one.index ?? 0) - (b.one.index ?? 0))
      .map(({ one, head }) => ({
        name: one[2],
        signature: squeeze(`${head}${head ? 'auto' : one[1]} ${one[2]}(${one[3]})${one[4] ? ' ' + one[4] : ''}`),
        doc: docBefore(text, (one.index ?? 0) + one[0].search(/\S/)),
      }))
    const group = groups.find(([, test]) => test(rel))?.[0] ?? 'Values'
    return { path: rel, namespace, group, structs, enums, aliases, constants, functions }
  })

for (const one of headers) {
  one.include = paint(`#include "dxfio/${one.path}"`)
  for (const fn of one.functions) fn.html = paint(fn.signature)
  for (const alias of one.aliases) alias.html = paint(`using ${alias.name} = ${alias.target};`)
  for (const item of one.structs) {
    if (item.template) item.templateHtml = paint(item.template)
    for (const method of item.methods) method.html = paint(method.signature)
  }
}

const kinds = [...names.entries()]
  .map(([type, dxf]) => ({
    type,
    dxf,
    family: type.startsWith('Entities::') ? 'entity' : type.startsWith('Objects::') ? 'object' : 'table',
  }))
  .sort((one, other) => one.dxf.localeCompare(other.dxf))

const homes = (type: string) => {
  const name = type.split('::').pop() ?? type
  return headers.find((one) => one.structs.some((item) => item.name === name) || one.aliases.some((item) => item.name === name))?.path
}
writeFileSync(join(import.meta.dirname, '..', 'src', 'headers.json'), JSON.stringify(headers))
const version = /project\(dxfio VERSION ([\d.]+)/.exec(readFileSync(join(root, 'CMakeLists.txt'), 'utf8'))?.[1] ?? ''
writeFileSync(join(import.meta.dirname, '..', 'src', 'version.json'), JSON.stringify({ version }))
writeFileSync(
  join(import.meta.dirname, '..', 'src', 'kinds.json'),
  JSON.stringify(kinds.map((one) => ({ ...one, header: homes(one.type) ?? 'dxfio.hpp' })), null, 1),
)
const declarations = headers.flatMap((one) => [
  ...one.structs.map((item) => ({ at: `${one.path} ${item.name}`, doc: item.doc })),
  ...one.structs.flatMap((item) =>
    [...item.fields.map((field) => ({ at: `${one.path} ${item.name}::${field.name}`, doc: field.doc })),
      ...item.methods.map((method) => ({ at: `${one.path} ${item.name}::${method.signature}`, doc: method.doc }))],
  ),
  ...one.enums.flatMap((item) => [{ at: `${one.path} ${item.name}`, doc: item.doc }, ...item.values.map((value) => ({ at: `${one.path} ${item.name}::${value.name}`, doc: value.doc }))]),
  ...one.aliases.map((item) => ({ at: `${one.path} ${item.name}`, doc: item.doc })),
  ...one.constants.map((item) => ({ at: `${one.path} ${item.name}`, doc: item.doc })),
  ...one.functions.map((item) => ({ at: `${one.path} ${item.name}`, doc: item.doc })),
])
const missing = declarations.filter((one) => !one.doc)
console.log(`${headers.length} headers, ${kinds.length} kinds, ${declarations.length - missing.length} of ${declarations.length} declarations documented`)
if (process.argv.includes('--missing')) {
  for (const one of missing) console.log(`  ${one.at}`)
}
