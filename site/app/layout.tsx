import { Analytics } from '@vercel/analytics/next'
import { SpeedInsights } from '@vercel/speed-insights/next'
import type { Metadata } from 'next'
import localFont from 'next/font/local'
import { Provider } from '@/components/provider'
import { description } from '@/lib/shared'
import { origin } from '@/scripts/origin.ts'
import './global.css'

const sans = localFont({
  src: [
    { path: '../public/fonts/Inter-Regular.woff2', weight: '400' },
    { path: '../public/fonts/Inter-SemiBold.woff2', weight: '600' },
  ],
  variable: '--font-inter',
})

const mono = localFont({
  src: [
    { path: '../public/fonts/JetBrainsMono-Regular.woff2', weight: '400' },
    { path: '../public/fonts/JetBrainsMono-SemiBold.woff2', weight: '600' },
  ],
  variable: '--font-jetbrains',
})

export const metadata: Metadata = {
  ...(origin ? { metadataBase: new URL(origin) } : {}),
  title: { default: 'dxfio: read and write DXF in C++', template: '%s · dxfio' },
  description,
  icons: { icon: '/favicon.svg' },
  openGraph: { siteName: 'dxfio', type: 'website', images: '/brand/dxfio-banner.png' },
}

export default function Layout({ children }: LayoutProps<'/'>) {
  return (
    <html lang="en" className={`${sans.variable} ${mono.variable}`} suppressHydrationWarning>
      <body className="flex min-h-screen flex-col">
        <Provider>{children}</Provider>
        <Analytics />
        <SpeedInsights />
      </body>
    </html>
  )
}
