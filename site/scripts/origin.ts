// Where the site is served, such as https://dxfio.dev/: SITE_URL, else the production address of the
// Vercel project that builds it. Each page names its full address from it, and every link starts from
// its path; with neither, the site is served at / and names no address.
const vercel = process.env.VERCEL_PROJECT_PRODUCTION_URL

export const origin = (process.env.SITE_URL ?? (vercel ? `https://${vercel}` : undefined))?.replace(/\/?$/, '/')
