import { ServerCodeBlock } from 'fumadocs-ui/components/codeblock.rsc'
import { DocsBody, DocsDescription, DocsPage, DocsTitle } from 'fumadocs-ui/layouts/glass/page'
import type { Metadata } from 'next'
import { notFound } from 'next/navigation'
import type { Field } from '@/scripts/api.ts'
import { describe, dxfName, find, headers, reference, slugsOf, toc } from '@/lib/reference'

const themes = { light: 'github-light', dark: 'github-dark' } as const

const Code = ({ code }: { code: string }) => <ServerCodeBlock code={code} lang="cpp" themes={themes} />

const Doc = ({ text }: { text: string }) => (text ? <p>{text}</p> : null)

function Fields({ fields }: { fields: Field[] }) {
  const defaults = fields.some((field) => field.value)
  return (
    <table>
      <thead>
        <tr>
          <th>Field</th>
          <th>{defaults ? 'Type and default' : 'Type'}</th>
          <th>Meaning</th>
        </tr>
      </thead>
      <tbody>
        {fields.map((field) => (
          <tr key={field.name}>
            <td>
              <code>{field.name}</code>
            </td>
            <td>
              <code>{field.type}</code>
              {field.value ? <code>{`{${field.value}}`}</code> : null}
            </td>
            <td>{field.doc}</td>
          </tr>
        ))}
      </tbody>
    </table>
  )
}

export default async function Page(props: PageProps<'/reference/[[...path]]'>) {
  const { path } = await props.params
  const one = find(path)
  if (!one) notFound()
  return (
    <DocsPage toc={toc(one)}>
      <DocsTitle>{one.path === 'dxfio.hpp' ? 'Reference' : one.path}</DocsTitle>
      <DocsDescription>
        {one.group} · in <code>{one.namespace}</code>
      </DocsDescription>
      <DocsBody>
        <Code code={`#include "dxfio/${one.path}"`} />

        {one.functions.length > 0 && (
          <section>
            <h2 id="functions">Functions</h2>
            {one.functions.map((fn) => (
              <div key={fn.signature}>
                <Code code={fn.signature} />
                <Doc text={fn.doc} />
              </div>
            ))}
          </section>
        )}

        {one.structs.map((item) => (
          <section key={item.name}>
            <h2 id={item.name}>
              {item.kind} {item.name}
              {dxfName.has(item.name) && <span className="ms-2 rounded-md border px-1.5 py-0.5 align-middle font-mono text-xs">{dxfName.get(item.name)}</span>}
            </h2>
            {item.template && <Code code={item.template} />}
            <Doc text={item.doc} />
            {item.fields.length > 0 && <Fields fields={item.fields} />}
            {item.methods.map((method) => (
              <div key={method.signature}>
                <Code code={method.signature} />
                <Doc text={method.doc} />
              </div>
            ))}
          </section>
        ))}

        {one.enums.map((item) => (
          <section key={item.name}>
            <h2 id={item.name}>enum class {item.name}</h2>
            <Doc text={item.doc} />
            <table>
              <tbody>
                {item.values.map((value) => (
                  <tr key={value.name}>
                    <td>
                      <code>{value.name}</code>
                    </td>
                    <td>{value.doc}</td>
                  </tr>
                ))}
              </tbody>
            </table>
          </section>
        ))}

        {one.aliases.map((item) => (
          <section key={item.name}>
            <h2 id={item.name}>using {item.name}</h2>
            <Doc text={item.doc} />
            <Code code={`using ${item.name} = ${item.target};`} />
          </section>
        ))}

        {one.constants.length > 0 && (
          <section>
            <h2 id="constants">Constants</h2>
            <table>
              <tbody>
                {one.constants.map((item) => (
                  <tr key={item.name} id={item.name}>
                    <td>
                      <code>{item.name}</code>
                    </td>
                    <td>
                      <code>
                        {item.type} = {item.value}
                      </code>
                    </td>
                    <td>{item.doc}</td>
                  </tr>
                ))}
              </tbody>
            </table>
          </section>
        )}
      </DocsBody>
    </DocsPage>
  )
}

export function generateStaticParams() {
  return headers.map((one) => ({ path: slugsOf(one.path) }))
}

export async function generateMetadata(props: PageProps<'/reference/[[...path]]'>): Promise<Metadata> {
  const { path } = await props.params
  const one = find(path)
  if (!one) notFound()
  const url = reference(one.path)
  return {
    title: one.path === 'dxfio.hpp' ? 'Reference' : `${one.path} reference`,
    description: describe(one),
    alternates: { canonical: url },
    openGraph: { url },
  }
}
