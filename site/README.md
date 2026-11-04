# dxfio site

The documentation website: Vite, TypeScript, React, Tailwind with
shadcn/ui on Base UI, GSAP for the hero, in dxfio's brand.

```sh
npm install
npm run dev        # http://localhost:5173
npm run build      # dist/, every page prerendered
npm run examples   # builds and runs every C++ example in ../docs/
```

## Writing the docs

The guide, the tutorials and the concepts are Markdown, one file a page,
in the repository's `docs/guide/`, `docs/tutorials/` and `docs/concepts/`;
`src/sections.ts` lists them in the order the sidebar shows them.
`docs/home.md` holds the home page's example. In a page, `#` is its
title, the text under it its lead, and each
`##` a section of the outline, its id given as `## Title {#id}`.
markdown-it compiles them when the site builds, and Shiki colors their
code in Ayu Light and Ayu Dark. A fence takes a title as
` ```cpp title="plan.cpp" `, and fences between `::: code-group` and
`:::` become tabs, each named in brackets: ` ```cmake [CMakeLists.txt] `.
A page links another by its address, such as `[Layers](/tutorials/layers/)`.

The fonts in `public/fonts/` hold Latin-1, general punctuation and ⌘ alone,
to stay small. A page that needs another character falls back to the
system's font for it until the fonts are cut again from Inter and JetBrains
Mono with that character's range added, with fontTools:

```sh
python3 -m fontTools.subset Inter-Regular.ttf --flavor=woff2 --layout-features='*' \
  --unicodes='U+0000-00FF,U+2000-206F,U+2318,U+FFFD' --output-file=Inter-Regular.woff2
```

## Publishing

Each page has one address, such as `/tutorials/layers/` or
`/reference/entities/line/`, and `npm run build` writes it to its own file,
`dist/tutorials/layers/index.html`, with its title, description and content
already in it; the app takes the page over once its script loads. Any static
host serves `dist/`, and `dist/404.html` is the page for an address with no
file. Build with the address the site is served at:

```sh
SITE_URL=https://dxfio.dev/ npm run build
```

Each page then names its full address as canonical, carries link previews
and breadcrumbs for search results, and `dist/sitemap.xml` lists them all. A
path in `SITE_URL`, as in `https://someone.github.io/dxfio/`, is where every
link starts.

Vercel needs no setting: with `SITE_URL` unset, the build takes the
project's production address, the custom domain once it has one.

## The examples

`npm run examples` needs `../build/release/lib/libdxfio.a`. The C++ of a
page is the body of a `main`, or a whole program when it has its own. It
must compile with warnings as errors and run; a ` ```text [Output] `
fence is what it must print, and every DXF it writes must read back
without an issue and pass the audit. `-- --acad` runs AutoCAD's own
audit on those files too, and names pick pages: `-- layers text`.

## The reference

`npm run api`, which `dev` and `build` run first, reads the `///` comments
of `../include/dxfio` and writes `src/headers.json` and `src/kinds.json`,
its signatures already colored. It prints how many declarations have a
comment; `node scripts/api.ts --missing` lists the ones that have none.
