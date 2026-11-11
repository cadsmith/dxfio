// Where the site is served, such as https://dxfio.dev/: SITE_URL, else the production address of the
// Vercel project that builds it. Each page names its full address from it; with neither, the site
// names no address. The site is served at a host's root: a path in SITE_URL is refused, not dropped.
const vercel = process.env.VERCEL_PROJECT_PRODUCTION_URL
const given = process.env.SITE_URL ?? (vercel ? `https://${vercel}` : undefined)

if (given && new URL(given).pathname !== '/') {
  throw new Error(`SITE_URL ${given} has a path; the site is served at a host's root, such as https://dxfio.dev/`)
}

export const origin = given ? new URL(given).origin + '/' : undefined
