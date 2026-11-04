import { mkdirSync, readFileSync, rmSync, writeFileSync } from 'node:fs'
import { join } from 'node:path'
import { pathToFileURL } from 'node:url'

type Crumb = { name: string; path: string }
type Head = { title: string; description: string; path: string; trail: Crumb[] }

const site = join(import.meta.dirname, '..')
const dist = join(site, 'dist')
const built = join(site, 'dist-ssr')

const { base, lost, pages, parse, render } = (await import(pathToFileURL(join(built, 'server.js')).href)) as {
  base: string
  lost: Head
  pages: Head[]
  parse: (path: string) => string[]
  render: (path: string) => Promise<string>
}

// SITE_URL, such as https://dxfio.dev/, gives each page the full address search engines index it by.
const origin = process.env.SITE_URL?.replace(/\/?$/, '/')
const at = (path: string) => new URL(path, origin).href

const escape = (text: string) =>
  text.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;').replace(/"/g, '&quot;')

const meta = (key: 'name' | 'property', name: string, content: string) =>
  `<meta ${key}="${name}" content="${escape(content)}" />`

const data = (value: object) =>
  `<script type="application/ld+json">${JSON.stringify({ '@context': 'https://schema.org', ...value }).replace(/</g, '\\u003c')}</script>`

const tags = (head: Head): string[] => {
  if (!origin) return []
  const own = at(head.path)
  return [
    `<link rel="canonical" href="${escape(own)}" />`,
    meta('property', 'og:type', 'website'),
    meta('property', 'og:site_name', 'dxfio'),
    meta('property', 'og:title', head.title),
    meta('property', 'og:description', head.description),
    meta('property', 'og:url', own),
    meta('property', 'og:image', at(`${base}brand/dxfio-banner.png`)),
    meta('name', 'twitter:card', 'summary_large_image'),
    head.trail.length > 0
      ? data({
          '@type': 'BreadcrumbList',
          itemListElement: head.trail.map((crumb, index) => ({
            '@type': 'ListItem',
            position: index + 1,
            name: crumb.name,
            item: at(crumb.path),
          })),
        })
      : data({ '@type': 'WebSite', name: 'dxfio', url: own }),
  ]
}

// Each page carries the stylesheet in itself: it paints from its own HTML with no request first,
// and the fonts the stylesheet names are fetched at once rather than after it.
const sheet = /<link rel="stylesheet" crossorigin href="([^"]+)">/
const linked = readFileSync(join(dist, 'index.html'), 'utf8')
const stylesheet = join(dist, sheet.exec(linked)?.[1].slice(base.length) ?? '')
// The app's code takes over a page already painted from its HTML, so it loads after what paints it.
const template = linked
  .replace(sheet, () => `<style>${readFileSync(stylesheet, 'utf8')}</style>`)
  .replace(/<(script type="module"|link rel="modulepreload")/g, (tag) => `${tag} fetchpriority="low"`)
rmSync(stylesheet)

const write = (file: string, head: Head, html: string, root: string, more: string[]) => {
  const description = head.description ? meta('name', 'description', head.description) : ''
  const page = template
    .replace(/<title>.*?<\/title>/, () => `<title>${escape(head.title)}</title>`)
    .replace(/<meta name="description"[^>]*>/, () => description)
    .replace('\n  </head>', () => `${more.map((tag) => `\n    ${tag}`).join('')}\n  </head>`)
    .replace('<div id="root"></div>', () => `<div id="root"${root}>${html}</div>`)
  mkdirSync(join(file, '..'), { recursive: true })
  writeFileSync(file, page)
}

for (const head of pages) {
  const html = await render(head.path)
  // The path the app reads back, to take over this page rather than draw it again.
  const root = ` data-path="${escape(parse(head.path).join('/'))}"`
  write(join(dist, head.path.slice(base.length), 'index.html'), head, html, root, tags(head))
}

write(join(dist, '404.html'), lost, await render(lost.path), '', [meta('name', 'robots', 'noindex')])

if (origin) {
  const urls = pages.map((head) => `  <url><loc>${escape(at(head.path))}</loc></url>\n`).join('')
  writeFileSync(
    join(dist, 'sitemap.xml'),
    `<?xml version="1.0" encoding="UTF-8"?>\n<urlset xmlns="http://www.sitemaps.org/schemas/sitemap/0.9">\n${urls}</urlset>\n`,
  )
}

// Crawlers read robots.txt only at a host's root.
if (base === '/') {
  writeFileSync(join(dist, 'robots.txt'), `User-agent: *\nAllow: /\n${origin ? `\nSitemap: ${at('/sitemap.xml')}\n` : ''}`)
}

// llms.txt: what the site holds, in Markdown, for a language model to read before its pages.
const home = pages.find((head) => head.path === base)
const sections = Map.groupBy(
  pages.filter((head) => head !== home),
  (head) => parse(head.path)[0],
)
const listed = [...sections].map(
  ([section, heads]) =>
    `## ${section[0].toUpperCase()}${section.slice(1)}\n\n${heads
      .map((head) => `- [${head.trail.at(-1)?.name}](${origin ? at(head.path) : head.path}): ${head.description}`)
      .join('\n')}\n`,
)
writeFileSync(join(dist, 'llms.txt'), `# dxfio\n\n> ${home?.description}\n\n${listed.join('\n')}`)

rmSync(built, { recursive: true })
console.log(`${pages.length} pages prerendered${origin ? ', each listed in sitemap.xml' : ''}`)
if (!origin) console.log('SITE_URL unset: no canonical addresses, link previews or sitemap')
