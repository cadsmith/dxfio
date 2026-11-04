import { useEffect, useMemo, useState } from 'react'
import { Badge } from '@/components/ui/badge'
import { Input } from '@/components/ui/input'
import { Table, TableBody, TableCell, TableHead, TableHeader, TableRow } from '@/components/ui/table'
import { headers, kinds, names, order, type Field, type Header } from '../api'
import { Layout, Sidebar, type Heading } from '../components/Layout'
import { Missing } from '../components/Missing'
import { Snippet } from '../components/Snippet'
import { missing, referenceHead } from '../head'
import { reach, reference, useTitle } from '../route'

const dxfName = new Map(kinds.map((one) => [one.type.split('::').pop() ?? one.type, one.dxf]))

const Doc = ({ text }: { text: string }) => (text ? <p className="doc-text">{text}</p> : null)

const short = (path: string) => path.replace(/\.hpp$/, '').replace(/^(entities|objects|tables|blocks|edit|audit|header)\//, '')

function Fields({ fields }: { fields: Field[] }) {
  const defaults = fields.some((field) => field.value)
  return (
    <div className="table-wrap">
      <Table className="fields">
        <TableHeader>
          <TableRow>
            <TableHead>Field</TableHead>
            <TableHead>{defaults ? 'Type and default' : 'Type'}</TableHead>
            <TableHead>Meaning</TableHead>
          </TableRow>
        </TableHeader>
        <TableBody>
          {fields.map((field) => (
            <TableRow key={field.name}>
              <TableCell>
                <code className="name">{field.name}</code>
              </TableCell>
              <TableCell>
                <code>{field.type}</code>
                {field.value ? <code className="default">{`{${field.value}}`}</code> : null}
              </TableCell>
              <TableCell className="meaning whitespace-normal">{field.doc}</TableCell>
            </TableRow>
          ))}
        </TableBody>
      </Table>
    </div>
  )
}

function Detail({ one }: { one: Header }) {
  return (
    <article className="detail">
      <header className="detail-head">
        <p className="eyebrow">{one.group}</p>
        <h1>
          {one.path
            .replace(/\.hpp$/, '')
            .split('/')
            .map((piece, index) => (
              <span key={`${piece}-${index}`}>
                {index > 0 && (
                  <>
                    /<wbr />
                  </>
                )}
                {piece}
              </span>
            ))}
        </h1>
        <Snippet html={one.include} code={`#include "dxfio/${one.path}"`} />
        <p className="faint">
          In <code>{one.namespace}</code>
        </p>
      </header>

      {one.functions.length > 0 && (
        <section className="decl" id="functions">
          <h2>Functions</h2>
          {one.functions.map((fn) => (
            <div className="member" key={fn.signature}>
              <Snippet html={fn.html} code={fn.signature} />
              <Doc text={fn.doc} />
            </div>
          ))}
        </section>
      )}

      {one.structs.map((item) => (
        <section className="decl" key={item.name} id={item.name}>
          <h2>
            <span className="decl-kind">{item.kind}</span> {item.name}
            {dxfName.has(item.name) && <Badge variant="secondary">{dxfName.get(item.name)}</Badge>}
          </h2>
          {item.template && <Snippet html={item.templateHtml} code={item.template} />}
          <Doc text={item.doc} />
          {item.fields.length > 0 && <Fields fields={item.fields} />}
          {item.methods.map((method) => (
            <div className="member" key={method.signature}>
              <Snippet html={method.html} code={method.signature} />
              <Doc text={method.doc} />
            </div>
          ))}
        </section>
      ))}

      {one.enums.map((item) => (
        <section className="decl" key={item.name} id={item.name}>
          <h2>
            <span className="decl-kind">enum class</span> {item.name}
          </h2>
          <Doc text={item.doc} />
          <div className="table-wrap">
            <Table className="fields">
              <TableBody>
                {item.values.map((value) => (
                  <TableRow key={value.name}>
                    <TableCell>
                      <code className="name">{value.name}</code>
                    </TableCell>
                    <TableCell className="meaning whitespace-normal">{value.doc}</TableCell>
                  </TableRow>
                ))}
              </TableBody>
            </Table>
          </div>
        </section>
      ))}

      {one.aliases.map((item) => (
        <section className="decl" key={item.name} id={item.name}>
          <h2>
            <span className="decl-kind">using</span> {item.name}
          </h2>
          <Doc text={item.doc} />
          <Snippet html={item.html} code={`using ${item.name} = ${item.target};`} />
        </section>
      ))}

      {one.constants.length > 0 && (
        <section className="decl" id="constants">
          <h2>Constants</h2>
          <div className="table-wrap">
            <Table className="fields">
              <TableBody>
                {one.constants.map((item) => (
                  <TableRow key={item.name} id={item.name}>
                    <TableCell>
                      <code className="name">{item.name}</code>
                    </TableCell>
                    <TableCell>
                      <code>
                        {item.type} = {item.value}
                      </code>
                    </TableCell>
                    <TableCell className="meaning whitespace-normal">{item.doc}</TableCell>
                  </TableRow>
                ))}
              </TableBody>
            </Table>
          </div>
        </section>
      )}
    </article>
  )
}

const matches = (one: Header, wanted: string): boolean =>
  one.path.toLowerCase().includes(wanted) ||
  names(one).some((name) => name.toLowerCase().includes(wanted)) ||
  one.structs.some((item) => (dxfName.get(item.name) ?? '').toLowerCase().includes(wanted))

const headings = (one: Header): Heading[] => {
  const at = (id: string, title: string) => ({ id, title, href: `${reference(one.path)}#${id}` })
  return [
    ...(one.functions.length > 0 ? [at('functions', 'Functions')] : []),
    ...one.structs.map((item) => at(item.name, item.name)),
    ...one.enums.map((item) => at(item.name, item.name)),
    ...one.aliases.map((item) => at(item.name, item.name)),
    ...(one.constants.length > 0 ? [at('constants', 'Constants')] : []),
  ]
}

export default function Reference({ route }: { route: string[] }) {
  const path = route.length > 0 ? `${route.join('/')}.hpp` : 'dxfio.hpp'
  const [query, setQuery] = useState('')
  const shown = useMemo(() => {
    const wanted = query.trim().toLowerCase()
    return wanted ? headers.filter((one) => matches(one, wanted)) : headers
  }, [query])
  const chosen = headers.find((one) => one.path === path)
  useTitle(chosen ? referenceHead(chosen).title : missing.title)
  useEffect(reach, [path])
  if (!chosen) return <Missing />
  const filter = (
    <div className="sidebar-filter">
      <label htmlFor="reference-filter">Filter the headers</label>
      <Input
        id="reference-filter"
        type="search"
        placeholder="circle, LWPOLYLINE, Add…"
        value={query}
        onChange={(event) => setQuery(event.target.value)}
      />
      {shown.length === 0 && <p className="faint">No header holds that name. Try a DXF name such as CIRCLE.</p>}
    </div>
  )
  const sidebar = (close: () => void) => (
    <Sidebar
      onPick={close}
      top={filter}
      groups={order
        .filter((group) => shown.some((one) => one.group === group))
        .map((group) => ({
          title: group,
          links: shown
            .filter((one) => one.group === group)
            .map((one) => ({ href: reference(one.path), label: short(one.path), current: one === chosen })),
        }))}
    />
  )
  return (
    <Layout sidebar={sidebar} outline={headings(chosen)}>
      <Detail one={chosen} />
    </Layout>
  )
}
