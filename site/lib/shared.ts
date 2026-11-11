import { createGetUrl } from 'fumadocs-core/source'

export const appName = 'dxfio'
export const description =
  'dxfio is a C++23 library that reads and writes AutoCAD DXF, ASCII and binary, every release from R10 to R2018, nothing lost.'
export const docsImageRoute = '/og'
export const docsContentRoute = '/llms.mdx'

export const gitConfig = { user: 'cadsmith', repo: 'dxfio', branch: 'main' }
export const repository = `https://github.com/${gitConfig.user}/${gitConfig.repo}`

// Each page has one address, ending in a slash: /tutorials/layers/.
export const pageUrl = (slugs: string[]) => (slugs.length > 0 ? `/${slugs.join('/')}/` : '/')

const getContentUrl = createGetUrl(docsContentRoute)

export function getPageMarkdownUrl(page: { slugs: string[]; locale?: string }) {
  const segments = [...page.slugs, 'content.md']
  return { segments, url: getContentUrl(segments, page.locale) }
}

const getImageUrl = createGetUrl(docsImageRoute)

export function getPageImageUrl(page: { slugs: string[]; locale?: string }) {
  const segments = [...page.slugs, 'image.png']
  return { segments, url: getImageUrl(segments, page.locale) }
}
