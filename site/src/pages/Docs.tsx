import { use, useEffect, useState, type ReactNode } from 'react'
import { Input } from '@/components/ui/input'
import { Tabs, TabsContent, TabsList, TabsTrigger } from '@/components/ui/tabs'
import { entries, find, groups, text } from '../docs'
import { docHead, missing } from '../head'
import { kinds } from '../kinds'
import { Layout, Sidebar } from '../components/Layout'
import { Missing } from '../components/Missing'
import { link, reach, reference, useTitle } from '../route'
import { titleOf } from '../sections'

const families = [
  { id: 'entity', label: 'Entities' },
  { id: 'object', label: 'Objects' },
  { id: 'table', label: 'Tables' },
] as const

function Kinds() {
  const [query, setQuery] = useState('')
  const wanted = query.trim().toLowerCase()
  const shown = (family: string) =>
    kinds.filter(
      (one) => one.family === family && (!wanted || `${one.dxf} ${one.type}`.toLowerCase().includes(wanted)),
    )
  return (
    <Tabs defaultValue="entity" className="kinds">
      <div className="kinds-bar">
        <TabsList>
          {families.map((one) => (
            <TabsTrigger key={one.id} value={one.id}>
              {one.label}
              <span className="kinds-count">{shown(one.id).length}</span>
            </TabsTrigger>
          ))}
        </TabsList>
        <Input
          type="search"
          className="sm:w-56"
          placeholder="Filter, such as MTEXT"
          aria-label="Filter the kinds"
          value={query}
          onChange={(event) => { setQuery(event.target.value) }}
        />
      </div>
      {families.map((family) => (
        <TabsContent key={family.id} value={family.id}>
          <ul className="kind-grid">
            {shown(family.id).map((one) => (
              <li key={one.dxf}>
                <a href={`${reference(one.header)}#${one.type.split('::').pop() ?? one.type}`}>
                  <span className="kind-dxf">
                    {one.dxf.split('_').map((piece, index) => (
                      <span key={`${piece}-${index}`}>
                        {index > 0 && (
                          <>
                            _<wbr />
                          </>
                        )}
                        {piece}
                      </span>
                    ))}
                  </span>
                  <span className="kind-type">{one.type.split('::').pop()}</span>
                </a>
              </li>
            ))}
          </ul>
          {shown(family.id).length === 0 && <p className="faint kinds-none">No {family.label.toLowerCase()} match that name.</p>}
        </TabsContent>
      ))}
    </Tabs>
  )
}

const extra: Record<string, ReactNode> = {
  kinds: <Kinds />,
}

const docsSidebar = (section: string, slug: string) => (close: () => void) => (
  <Sidebar
    onPick={close}
    groups={groups.map((group) => ({
      title: group.title,
      links: group.slugs.map((one) => ({
        href: link(group.id, one),
        label: find(group.id, one)?.page.title ?? one,
        current: group.id === section && one === slug,
      })),
    }))}
  />
)

export default function Docs({ route }: { route: string[] }) {
  const entry = route.length <= 2 ? find(route[0], route[1]) : undefined
  useTitle(entry ? docHead(entry).title : missing.title)
  useEffect(reach, [entry])
  if (!entry) return <Missing />
  const page = use(text(entry))
  const own = link(entry.section, entry.slug)
  const index = entries.indexOf(entry)
  const previous = entries[index - 1]
  const next = entries[index + 1]
  return (
    <Layout
      sidebar={docsSidebar(entry.section, entry.slug)}
      outline={page.parts.map((one) => ({ id: one.id, title: one.title, href: `${own}#${one.id}` }))}
    >
      <article className="doc">
        <header className="doc-head">
          <p className="eyebrow">{titleOf(entry.section)}</p>
          <h1>{page.title}</h1>
          <div className="lead" dangerouslySetInnerHTML={{ __html: page.lead }} />
        </header>
        {page.parts.map((one) => (
          <section key={one.id} id={one.id} className="doc-part">
            <h2>
              <a href={`${own}#${one.id}`}>{one.title}</a>
            </h2>
            <div className="prose" dangerouslySetInnerHTML={{ __html: one.html }} />
            {extra[one.id]}
          </section>
        ))}
        <nav className="pager" aria-label="Pages">
          {previous ? (
            <a className="pager-link" href={link(previous.section, previous.slug)}>
              <span>Previous page</span>
              <strong>{previous.page.title}</strong>
            </a>
          ) : (
            <span />
          )}
          {next ? (
            <a className="pager-link next" href={link(next.section, next.slug)}>
              <span>Next page</span>
              <strong>{next.page.title}</strong>
            </a>
          ) : (
            <a className="pager-link next" href={link('reference')}>
              <span>Next</span>
              <strong>Reference</strong>
            </a>
          )}
        </nav>
      </article>
    </Layout>
  )
}
