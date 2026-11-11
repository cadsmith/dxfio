import type { BlockContent, Heading, Root, RootContent, Table, TableRow } from 'mdast'
import type { MdxJsxFlowElement } from 'mdast-util-mdx-jsx'
import kinds from '../generated/kinds.json' with { type: 'json' }
import type { Kind } from '../scripts/api.ts'
import { reference } from './reference'

// GitHub's alerts, as `> [!NOTE]`, become the site's callouts, so a page reads the same on GitHub.
const alerts: Record<string, 'info' | 'warn' | 'error'> = {
  NOTE: 'info',
  TIP: 'info',
  IMPORTANT: 'warn',
  WARNING: 'warn',
  CAUTION: 'error',
}

export function remarkAlerts() {
  return (tree: Root) => {
    tree.children = tree.children.map((node): RootContent => {
      if (node.type !== 'blockquote') return node
      const [first, ...rest] = node.children
      const lead = first?.type === 'paragraph' ? first.children[0] : undefined
      const marker = lead?.type === 'text' ? /^\[!(\w+)\]\n?/.exec(lead.value) : null
      const type = marker ? alerts[marker[1] ?? ''] : undefined
      if (!first || first.type !== 'paragraph' || lead?.type !== 'text' || !marker || !type) return node
      const text = lead.value.slice(marker[0].length)
      const body = text ? [{ ...lead, value: text }, ...first.children.slice(1)] : first.children.slice(1)
      const callout: MdxJsxFlowElement = {
        type: 'mdxJsxFlowElement',
        name: 'Callout',
        attributes: [{ type: 'mdxJsxAttribute', name: 'type', value: type }],
        children: [...(body.length > 0 ? [{ ...first, children: body }] : []), ...rest] as BlockContent[],
      }
      return callout
    })
  }
}

const families = [
  { id: 'entity', title: 'Entities' },
  { id: 'object', title: 'Objects' },
  { id: 'table', title: 'Tables' },
] as const

const cell = (children: TableRow['children'][number]['children']) => ({ type: 'tableCell' as const, children })

// The kinds with a type, by family, at the end of the section headed `[#kinds]`, each linked to its reference.
const kindTables = (): RootContent[] =>
  families.flatMap(({ id, title }): RootContent[] => {
    const rows: TableRow[] = (kinds as Kind[])
      .filter((one) => one.family === id)
      .map((one) => {
        const name = one.type.split('::').pop() ?? one.type
        return {
          type: 'tableRow',
          children: [
            cell([{ type: 'inlineCode', value: one.dxf }]),
            cell([{ type: 'link', url: `${reference(one.header)}#${name}`, children: [{ type: 'inlineCode', value: one.type }] }]),
          ],
        }
      })
    const heading: Heading = { type: 'heading', depth: 3, children: [{ type: 'text', value: `${title} [#kinds-${id}]` }] }
    const table: Table = {
      type: 'table',
      align: ['left', 'left'],
      children: [{ type: 'tableRow', children: [cell([{ type: 'text', value: 'DXF' }]), cell([{ type: 'text', value: 'Type' }])] }, ...rows],
    }
    return [heading, table]
  })

export function remarkKinds() {
  return (tree: Root) => {
    const at = tree.children.findIndex(
      (node) => node.type === 'heading' && node.children.some((one) => one.type === 'text' && one.value.endsWith('[#kinds]')),
    )
    if (at < 0) return
    const next = tree.children.findIndex((node, index) => index > at && node.type === 'heading' && node.depth <= 2)
    tree.children.splice(next < 0 ? tree.children.length : next, 0, ...kindTables())
  }
}
