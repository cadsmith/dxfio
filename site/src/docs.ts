import { sections } from './sections'

export type Part = { id: string; title: string; html: string }
export type Page = { title: string; lead: string; parts: Part[] }
export type Outline = { title: string; lead: string; parts: Omit<Part, 'html'>[] }

// Every page's outline, for the sidebar, the pager and the search; a page's text comes with the page.
const outlines = import.meta.glob<{ default: Outline }>('../../docs/{guide,tutorials,concepts}/*.md', {
  eager: true,
  query: '?outline',
})
const texts = import.meta.glob<{ default: Page }>('../../docs/{guide,tutorials,concepts}/*.md')

const path = (section: string, slug: string) => `../../docs/${section}/${slug}.md`

export const groups = sections.map((group) => ({ ...group, slugs: group.slugs.filter((slug) => path(group.id, slug) in outlines) }))

export type Entry = { section: string; slug: string; page: Outline }

export const entries: Entry[] = groups.flatMap((group) =>
  group.slugs.flatMap((slug) => {
    const outline = outlines[path(group.id, slug)]
    return outline ? [{ section: group.id, slug, page: outline.default }] : []
  }),
)

// A section alone opens its first page.
export const find = (section: string, slug?: string): Entry | undefined =>
  entries.find((one) => one.section === section && (slug === undefined || one.slug === slug))

const fetched = new Map<Entry, Promise<Page>>()

// The page's whole text, fetched once, for use() to wait on.
export const text = (entry: Entry): Promise<Page> => {
  const known = fetched.get(entry)
  if (known) return known
  const load = texts[path(entry.section, entry.slug)]
  if (!load) return Promise.reject(new Error(`no page at ${entry.section}/${entry.slug}`))
  const loading = load().then((module) => module.default)
  fetched.set(entry, loading)
  return loading
}
