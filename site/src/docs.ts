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

export const groups = sections.map((group) => ({ ...group, slugs: group.slugs.filter((slug) => outlines[path(group.id, slug)]) }))

export type Entry = { section: string; slug: string; page: Outline }

export const entries: Entry[] = groups.flatMap((group) =>
  group.slugs.map((slug) => ({ section: group.id, slug, page: outlines[path(group.id, slug)].default })),
)

// A section alone opens its first page.
export const find = (section: string, slug?: string): Entry | undefined =>
  entries.find((one) => one.section === section && (slug === undefined || one.slug === slug))

const fetched = new Map<Entry, Promise<Page>>()

// The page's whole text, fetched once, for use() to wait on.
export const text = (entry: Entry): Promise<Page> => {
  const known = fetched.get(entry)
  if (known) return known
  const loading = texts[path(entry.section, entry.slug)]().then((module) => module.default)
  fetched.set(entry, loading)
  return loading
}
