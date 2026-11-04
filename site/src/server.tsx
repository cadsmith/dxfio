import { prerender } from 'react-dom/static'
import { App } from './App'
import { headers } from './api'
import { entries } from './docs'
import { docHead, home, missing, referenceHead } from './head'
import { link, parse } from './route'

export { parse }

export const base = import.meta.env.BASE_URL

// Every page of the site, each written to its own file.
export const pages = [home, ...entries.map(docHead), ...headers.map(referenceHead)]

// The page a host serves for an address it has no file for.
export const lost = { ...missing, path: link('not-found') }

// A page is written whole where it stands: a larger chunk size leaves no part of it to a hidden
// block a script swaps in later, after the page has already painted its fallback.
export const render = async (path: string) => {
  const { prelude } = await prerender(<App at={parse(path)} />, { progressiveChunkSize: Number.POSITIVE_INFINITY })
  return new Response(prelude).text()
}
