import path from 'node:path'
import { createMDX } from 'fumadocs-mdx/next'

const withMDX = createMDX()

/** @type {import('next').NextConfig} */
const config = {
  // Static files a host serves as they are: out/tutorials/layers/index.html for /tutorials/layers/.
  output: 'export',
  trailingSlash: true,
  reactStrictMode: true,
  // The pages are in the repository's docs/, beside the site.
  turbopack: { root: path.join(import.meta.dirname, '..') },
}

export default withMDX(config)
