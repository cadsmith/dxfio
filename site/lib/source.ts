import { llms, loader } from 'fumadocs-core/source'
import { metaSchema, pageSchema } from 'fumadocs-core/source/schema'
import { applyMdxPreset } from 'fumadocs-mdx/config'
import { defineCollections, defineDocs } from 'fumadocs-mdx/macro'
import { remarkAlerts, remarkKinds } from './markdown'
import { pageUrl } from './shared'

// The pages are Markdown in the repository's docs/, beside the site; its guidelines there are not pages.
const mdxOptions = applyMdxPreset({
  remarkPlugins: (plugins) => [remarkAlerts, remarkKinds, ...plugins],
})

const docs = defineDocs({
  dir: '../docs',
  docs: {
    files: ['guide/*.md', 'tutorials/*.md', 'concepts/*.md'],
    schema: pageSchema,
    postprocess: { includeProcessedMarkdown: true },
    mdxOptions,
  },
  meta: { files: ['meta.json', '*/meta.json'], schema: metaSchema },
})

// The home page's install and example, which the examples script also builds and runs.
export const home = defineCollections({ type: 'doc', dir: '../docs', files: ['home.md'], schema: pageSchema, mdxOptions })

export const source = loader({ baseUrl: '/', url: pageUrl, source: docs.toFumadocsSource(), plugins: [] })

export const docsLlms = llms(source, {
  renderPage: async (page) => `# ${page.data.title} (${page.url})\n\n${await page.data.getText('processed')}`,
})

// A page's lead, its first paragraph, is its description.
export const leadOf = (page: { data: { structuredData: { contents: { heading?: string; content: string }[] } } }) =>
  page.data.structuredData.contents.find((one) => !one.heading)?.content ?? ''
