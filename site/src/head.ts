import type { Header } from './api'
import type { Entry } from './docs'
import { link, reference } from './route'
import { start, titleOf } from './sections'

type Crumb = { name: string; path: string }

// What a page tells a search engine: its title, a summary, its one address and where it sits.
export type Head = { title: string; description: string; path: string; trail: Crumb[] }

// The text without its tags, each from its < to the > that closes it.
const untagged = (html: string): string => {
  let out = ''
  let at = 0
  while (at < html.length) {
    const open = html.indexOf('<', at)
    if (open < 0) break
    const close = html.indexOf('>', open + 1)
    if (close < 0) break
    out += html.slice(at, open)
    at = close + 1
  }
  return out + html.slice(at)
}

const plain = (html: string) =>
  untagged(html)
    .replace(/\s+/g, ' ')
    .replace(/&lt;/g, '<')
    .replace(/&gt;/g, '>')
    .replace(/&quot;/g, '"')
    .replace(/&amp;/g, '&')
    .trim()

export const home: Head = {
  title: 'dxfio: read and write DXF in C++',
  description:
    'dxfio is a C++23 library that reads and writes AutoCAD DXF, ASCII and binary, every release from R10 to R2018, nothing lost.',
  path: link(),
  trail: [],
}

export const missing: Head = { title: 'Page not found · dxfio', description: '', path: '', trail: [] }

export const docHead = ({ section, slug, page }: Entry): Head => {
  const own = { name: page.title, path: link(section, slug) }
  return {
    title: `${page.title} · dxfio`,
    description: plain(page.lead),
    path: own.path,
    trail: start(section) === own.path ? [own] : [{ name: titleOf(section), path: start(section) }, own],
  }
}

export const referenceHead = (one: Header): Head => {
  const top = { name: 'Reference', path: link('reference') }
  if (one.path === 'dxfio.hpp') {
    return {
      title: 'Reference · dxfio',
      description: 'Every type and function of dxfio, from its headers: the fields, groups and defaults of each DXF kind.',
      path: top.path,
      trail: [top],
    }
  }
  const declared = [...one.structs, ...one.enums, ...one.aliases, ...one.functions, ...one.constants]
  const summary = declared.find((item) => item.doc)?.doc
  return {
    title: `${one.path} · dxfio reference`,
    description: summary ?? `What dxfio/${one.path} declares: ${[...new Set(declared.map((item) => item.name))].join(', ')}.`,
    path: reference(one.path),
    trail: [top, { name: one.path, path: reference(one.path) }],
  }
}
