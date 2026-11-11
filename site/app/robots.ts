import type { MetadataRoute } from 'next'
import { origin } from '@/scripts/origin.ts'

export const dynamic = 'force-static'

export default function robots(): MetadataRoute.Robots {
  return { rules: { userAgent: '*', allow: '/' }, ...(origin ? { sitemap: new URL('/sitemap.xml', origin).href } : {}) }
}
