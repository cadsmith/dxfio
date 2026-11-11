import type { MetadataRoute } from 'next'
import { origin } from '@/scripts/origin.ts'
import { headers, reference } from '@/lib/reference'
import { source } from '@/lib/source'

export const dynamic = 'force-static'

// Every page's full address; with no address to name, there is no sitemap to give.
export default function sitemap(): MetadataRoute.Sitemap {
  if (!origin) return []
  const at = (path: string) => new URL(path, origin).href
  return [at('/'), ...source.getPages().map((page) => at(page.url)), ...headers.map((one) => at(reference(one.path)))].map((url) => ({ url }))
}
