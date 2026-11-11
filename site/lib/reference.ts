import type * as PageTree from 'fumadocs-core/page-tree'
import data from '../generated/headers.json' with { type: 'json' }
import kinds from '../generated/kinds.json' with { type: 'json' }
import type { Header, Kind } from '../scripts/api.ts'

export const headers = data as Header[]

export const order = ['Start', 'Edit', 'Document', 'Entities', 'Objects', 'Tables', 'Blocks', 'Audit', 'Values']

// A header's page; the one that includes them all is the reference's own.
export const reference = (path: string) => (path === 'dxfio.hpp' ? '/reference/' : `/reference/${path.replace(/\.hpp$/, '')}/`)

export const slugsOf = (path: string) => (path === 'dxfio.hpp' ? [] : path.replace(/\.hpp$/, '').split('/'))

export const find = (slugs: string[] = []) => headers.find((one) => one.path === (slugs.length > 0 ? `${slugs.join('/')}.hpp` : 'dxfio.hpp'))

const short = (path: string) => path.replace(/\.hpp$/, '').replace(/^(entities|objects|tables|blocks|edit|audit|header)\//, '')

export const dxfName = new Map((kinds as Kind[]).map((one) => [one.type.split('::').pop() ?? one.type, one.dxf]))

// What a header's page tells a search engine: its first documented declaration, else the names it declares.
export const describe = (one: Header): string => {
  if (one.path === 'dxfio.hpp') return 'Every type and function of dxfio, from its headers: the fields, groups and defaults of each DXF kind.'
  const declared = [...one.structs, ...one.enums, ...one.aliases, ...one.functions, ...one.constants]
  return declared.find((item) => item.doc)?.doc ?? `What dxfio/${one.path} declares: ${[...new Set(declared.map((item) => item.name))].join(', ')}.`
}

// The outline of a header's page, one entry for each section it draws.
export const toc = (one: Header) => [
  ...(one.functions.length > 0 ? [{ title: 'Functions', url: '#functions', depth: 2 }] : []),
  ...[...one.structs, ...one.enums, ...one.aliases].map((item) => ({ title: item.name, url: `#${item.name}`, depth: 2 })),
  ...(one.constants.length > 0 ? [{ title: 'Constants', url: '#constants', depth: 2 }] : []),
]

// The reference in the sidebar: a folder of groups, each a folder of its headers.
export const referenceTree = (): PageTree.Folder => ({
  type: 'folder',
  name: 'Reference',
  defaultOpen: false,
  index: { type: 'page', name: 'Reference', url: reference('dxfio.hpp') },
  children: order
    .filter((group) => headers.some((one) => one.group === group && one.path !== 'dxfio.hpp'))
    .map((group) => ({
      type: 'folder',
      name: group,
      children: headers
        .filter((one) => one.group === group && one.path !== 'dxfio.hpp')
        .map((one) => ({ type: 'page', name: short(one.path), url: reference(one.path) })),
    })),
})

// A header's page for search: each declaration a heading, its documentation and its fields' the text under it.
export const searchIndex = (one: Header) => ({
  id: reference(one.path),
  url: reference(one.path),
  title: one.path === 'dxfio.hpp' ? 'Reference' : one.path,
  description: describe(one),
  breadcrumbs: ['Reference', one.group],
  structuredData: {
    headings: toc(one).map((entry) => ({ id: entry.url.slice(1), content: entry.title })),
    contents: [
      ...one.functions.map((fn) => ({ heading: 'functions', content: `${fn.name}: ${fn.doc}` })),
      ...one.structs.flatMap((item) => [
        { heading: item.name, content: [dxfName.get(item.name), item.doc].filter(Boolean).join(' ') },
        ...item.fields.map((field) => ({ heading: item.name, content: `${field.name}: ${field.doc}` })),
        ...item.methods.map((method) => ({ heading: item.name, content: `${method.signature}: ${method.doc}` })),
      ]),
      ...one.enums.map((item) => ({ heading: item.name, content: `${item.doc} ${item.values.map((value) => value.name).join(', ')}` })),
      ...one.aliases.map((item) => ({ heading: item.name, content: item.doc })),
      ...one.constants.map((item) => ({ heading: 'constants', content: `${item.name}: ${item.doc}` })),
    ],
  },
})
