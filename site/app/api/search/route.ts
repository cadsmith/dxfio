import { createSearchAPI } from 'fumadocs-core/search/server'
import { headers, searchIndex } from '@/lib/reference'
import { leadOf, source } from '@/lib/source'

export const revalidate = false

// The pages and every header of the reference, searched in the browser from one static index.
export const { staticGET: GET } = createSearchAPI('advanced', {
  language: 'english',
  indexes: [
    ...source.getPages().map((page) => ({
      id: page.url,
      url: page.url,
      title: page.data.title,
      description: page.data.description ?? leadOf(page),
      structuredData: page.data.structuredData,
    })),
    ...headers.map(searchIndex),
  ],
})
