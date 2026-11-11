import { fromHighlighter } from '@shikijs/markdown-it/core'
import MarkdownIt from 'markdown-it'
import attrs from 'markdown-it-attrs'
import spans from 'markdown-it-bracketed-spans'
import alerts from 'markdown-it-github-alerts'
import type { Plugin } from 'vite'
import { figure, highlighter, labelOf, themes } from './highlight.ts'

type Part = { id: string; title: string; html: string }

// No HTML from the pages themselves: a class on a run of text is written as [text]{.class}.
const md = MarkdownIt({ html: false, typographer: true })
// The spans plugin is CommonJS, so its default import is the plugin itself under Node and a namespace to TypeScript.
md.use(spans as unknown as Parameters<typeof md.use>[0]).use(attrs)
md.use(alerts)
md.use(fromHighlighter(highlighter, { themes, defaultColor: false, transformers: [figure] }))
md.renderer.rules.table_open = () => '<div class="table-scroll"><table>\n'
md.renderer.rules.table_close = () => '</table></div>\n'

// A page links another as /tutorials/layers/, below wherever the site is served.
let base = '/'
md.renderer.rules.link_open = (tokens, index, options, _env, self) => {
  const token = tokens[index]
  const href = token?.attrGet('href')
  if (typeof href === 'string' && href.startsWith('/') && !href.startsWith('//')) token?.attrSet('href', base + href.slice(1))
  return self.renderToken(tokens, index, options)
}

const slug = (title: string) =>
  title
    .toLowerCase()
    .replace(/[^a-z0-9]+/g, '-')
    .replace(/^-|-$/g, '')

const escape = (text: string) => text.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;')

let groups = 0

// Where the line opens that the backticks at the index stand on, indented by up to the three spaces CommonMark
// allows; nothing when they stand elsewhere in a line.
const lined = (body: string, at: number): number | undefined => {
  let start = at
  while (start > 0 && at - start < 3 && body[start - 1] === ' ') start -= 1
  return start === 0 || body[start - 1] === '\n' ? start : undefined
}

// Where the next fence opens at or after the index: three backticks opening a line with a language word right
// after them, as ```cpp; none for backticks inside prose.
const opening = (body: string, from: number): number => {
  let at = body.indexOf('```', from)
  while (at >= 0) {
    if (lined(body, at) !== undefined && /^\w/.test(body.slice(at + 3, at + 4))) return at
    at = body.indexOf('```', at + 3)
  }
  return -1
}

// Each fenced block of the body, from its opening line's start so that an indented fence keeps its indentation
// for the renderer to take off: the whole of it, its language and the rest of its opening line as the meta.
const fenced = (body: string): [string, string, string][] => {
  const out: [string, string, string][] = []
  let at = opening(body, 0)
  while (at >= 0) {
    const line = body.indexOf('\n', at)
    const end = line < 0 ? -1 : body.indexOf('```', line + 1)
    if (end < 0) break
    const [, lang, meta] = /^(\w+)([^\n]*)/.exec(body.slice(at + 3, line)) ?? []
    if (lang !== undefined && meta !== undefined) out.push([body.slice(lined(body, at) ?? at, end + 3), lang, meta])
    at = opening(body, end + 3)
  }
  return out
}

const group = (body: string): string => {
  const fences = fenced(body)
  const id = `group-${++groups}`
  const tabs = fences
    .map(
      ([, lang, meta], index) =>
        `<button type="button" role="tab" data-tab="${index}" aria-selected="${index === 0}">${escape(labelOf(lang, meta))}</button>`,
    )
    .join('')
  const blocks = fences
    .map(([whole], index) => `<div class="code-block" data-block="${index}"${index === 0 ? '' : ' hidden'}>${md.render(whole)}</div>`)
    .join('')
  return `<div class="code-group" id="${id}"><div class="code-tabs" role="tablist">${tabs}</div>${blocks}</div>`
}

// A code group is rendered apart and stands in for a paragraph of its own, blank lines around it so that the prose
// after it never joins it, until the page is rendered.
const render = (source: string): string => {
  const held: string[] = []
  const marked = source.replace(/^::: code-group\n([^]*?)\n:::\s*$/gm, (_, body: string) => {
    held.push(group(body))
    return `\n\n%%group ${held.length - 1}%%\n\n`
  })
  return md.render(marked).replace(/<p>%%group (\d+)%%<\/p>\n/g, (_, index: string) => held[Number(index)] ?? '')
}

export const page = (source: string) => {
  const lines = source.split('\n')
  const title = lines.find((line) => line.startsWith('# '))?.slice(2).trim() ?? ''
  const parts: Part[] = []
  const lead: string[] = []
  let current: { id: string; title: string; lines: string[] } | undefined
  const close = () => {
    if (current) parts.push({ id: current.id, title: current.title, html: render(current.lines.join('\n')) })
  }
  for (const line of lines) {
    if (line.startsWith('# ')) continue
    if (line.startsWith('## ')) {
      close()
      let title = line.slice(3).trim()
      const tagged = /\s\{#([\w-]+)\}$/.exec(title)
      if (tagged) title = title.slice(0, tagged.index).trimEnd()
      current = { id: tagged?.[1] ?? slug(title), title, lines: [] }
      continue
    }
    if (current) current.lines.push(line)
    else lead.push(line)
  }
  close()
  return { title, lead: render(lead.join('\n')), parts }
}

export const markdown = (): Plugin => ({
  name: 'dxfio-markdown',
  configResolved(config) {
    base = config.base
  },
  // A page imported with ?outline keeps its sections' names and drops their text.
  transform(source, id) {
    const [file = id, query] = id.split('?')
    if (!file.endsWith('.md')) return null
    const whole = page(source)
    const kept = query === 'outline' ? { ...whole, parts: whole.parts.map(({ id: part, title }) => ({ id: part, title })) } : whole
    return { code: `export default ${JSON.stringify(kept)}`, map: null }
  },
})
