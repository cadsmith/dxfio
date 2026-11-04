import { existsSync, readFileSync } from 'node:fs'
import path from 'node:path'
import tailwindcss from '@tailwindcss/vite'
import react from '@vitejs/plugin-react'
import { defineConfig, type Plugin } from 'vite'
import { markdown } from './scripts/markdown.ts'
import { origin } from './scripts/origin.ts'

// The preview answers as a static host does: an address with no file gets 404.html and a 404, not
// the home page, so what Lighthouse and crawlers find locally is what they find once it is live.
const hosted = (): Plugin => ({
  name: 'dxfio-hosted',
  configurePreviewServer(server) {
    const out = path.resolve(server.config.root, server.config.build.outDir)
    server.middlewares.use((request, response, next) => {
      const address = decodeURIComponent(new URL(request.url ?? '/', 'http://host').pathname)
      const file = path.join(out, address.slice(server.config.base.length - 1))
      if (existsSync(address.endsWith('/') ? path.join(file, 'index.html') : file)) return next()
      response.statusCode = 404
      response.setHeader('content-type', 'text/html')
      response.end(readFileSync(path.join(out, '404.html')))
    })
  },
})

export default defineConfig({
  base: origin ? new URL(origin).pathname : '/',
  plugins: [react(), tailwindcss(), markdown(), hosted()],
  // The pages are Markdown in the repository's docs/, beside the site.
  server: { fs: { allow: [import.meta.dirname, path.resolve(import.meta.dirname, '../docs')] } },
  resolve: {
    alias: {
      '@': path.resolve(import.meta.dirname, './src'),
    },
  },
})
