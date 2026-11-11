# dxfio site

The documentation website: Next.js and Fumadocs, built to static files, in
dxfio's colors and fonts.

```sh
npm install
npm run dev        # http://localhost:3000
npm run build      # out/, every page a static file
npm run preview    # serves out/
npm run examples   # builds and runs every C++ example in ../docs/
```

## Writing the docs

The guide, the tutorials and the concepts are Markdown, one file a page, in
the repository's `docs/guide/`, `docs/tutorials/` and `docs/concepts/`; each
folder's `meta.json` orders its pages in the sidebar and groups them under
`---Separators---`. `docs/home.md` holds the home page's install and
example. A page's `title:` front matter is its title, its first paragraph
its lead and description, and each `##` a section of the outline, its id
given as `## Title [#id]`.

A fence takes a title as ` ```cpp title="plan.cpp" `, and fences next to
each other with a `tab="…"` become tabs: ` ```cmake tab="CMakeLists.txt" `.
GitHub's alerts, `> [!NOTE]`, `> [!IMPORTANT]` and `> [!CAUTION]`, become
callouts. A page links another by its address, such as
`[Layers](/tutorials/layers/)`. The section headed `[#kinds]` gets the
table of every kind with a type at its end, from `generated/kinds.json`.

## Publishing

Each page has one address, such as `/tutorials/layers/` or
`/reference/entities/line/`, and `npm run build` writes it to its own file,
`out/tutorials/layers/index.html`. Any static host serves `out/`, and
`out/404.html` is the page for an address with no file. Build with the
address the site is served at:

```sh
SITE_URL=https://dxfio.dev/ npm run build
```

Each page then names its full address as canonical, and `out/sitemap.xml`
lists them all. The site is served at a host's root: a `SITE_URL` with a
path stops the build rather than linking every page wrong. Vercel needs no setting: with `SITE_URL` unset, the build
takes the project's production address. Every page also has its Markdown at
`/llms.mdx/<page>/content.md`, a card image at `/og/<page>/image.png`, and
`/llms.txt` lists them for a language model.

## The examples

`npm run examples` needs `../build/release/lib/libdxfio.a`. The C++ of a
page is the body of a `main`, or a whole program when it has its own. It
must compile with warnings as errors and run; a ` ```text title="Output" `
fence is what it must print, and every DXF it writes must read back without
an issue and pass the audit. `-- --acad` runs AutoCAD's own audit on those
files too, and names pick pages: `-- layers text`.

## The reference

`npm run api`, which `dev` and `build` run first, reads the `///` comments
of `../include/dxfio` and writes `generated/headers.json` and
`generated/kinds.json`, from which each header gets its page and its
entries in the search. It prints how many declarations have a comment;
`node scripts/api.ts --missing` lists the ones that have none.
