import { DocsBody, DocsPage, DocsTitle, MarkdownCopyButton, ViewOptionsPopover } from 'fumadocs-ui/layouts/glass/page'
import { createRelativeLink } from 'fumadocs-ui/mdx'
import type { Metadata } from 'next'
import { notFound } from 'next/navigation'
import { getMDXComponents } from '@/components/mdx'
import { getPageImageUrl, getPageMarkdownUrl, gitConfig } from '@/lib/shared'
import { leadOf, source } from '@/lib/source'

export default async function Page(props: PageProps<'/[...slug]'>) {
  const { slug } = await props.params
  const page = source.getPage(slug)
  if (!page) notFound()
  const MDX = page.data.body
  const markdownUrl = getPageMarkdownUrl(page).url
  return (
    <DocsPage toc={page.data.toc} full={page.data.full}>
      <DocsTitle>{page.data.title}</DocsTitle>
      <div className="flex flex-row items-center gap-2 border-b pb-6">
        <MarkdownCopyButton markdownUrl={markdownUrl} />
        <ViewOptionsPopover
          markdownUrl={markdownUrl}
          githubUrl={`https://github.com/${gitConfig.user}/${gitConfig.repo}/blob/${gitConfig.branch}/docs/${encodeURI(page.path)}`}
        />
      </div>
      <DocsBody>
        <MDX components={getMDXComponents({ a: createRelativeLink(source, page) })} />
      </DocsBody>
    </DocsPage>
  )
}

export function generateStaticParams() {
  return source.generateParams().filter((one) => one.slug.length > 0)
}

export async function generateMetadata(props: PageProps<'/[...slug]'>): Promise<Metadata> {
  const { slug } = await props.params
  const page = source.getPage(slug)
  if (!page) notFound()
  const description = page.data.description ?? leadOf(page)
  return {
    title: page.data.title,
    description,
    alternates: { canonical: page.url },
    openGraph: { url: page.url, images: getPageImageUrl(page).url },
  }
}
