import { createHighlighter, type ShikiTransformer } from 'shiki'
import type { Element, ElementContent } from 'hast'

export const themes = { light: 'ayu-light', dark: 'ayu-dark' } as const

const labels: Record<string, string> = { cpp: 'C++', cmake: 'CMake', sh: 'Shell', shellscript: 'Shell' }

const element = (tagName: string, properties: Element['properties'], children: ElementContent[]): Element => ({
  type: 'element',
  tagName,
  properties,
  children,
})

const text = (value: string): ElementContent => ({ type: 'text', value })

// What stands between two marks in the meta, as [a label] or title="a title"; nothing when it is empty or absent.
const between = (meta: string, open: string, close: string): string | undefined => {
  const from = meta.indexOf(open)
  if (from < 0) return undefined
  const to = meta.indexOf(close, from + open.length)
  return to > from + open.length ? meta.slice(from + open.length, to) : undefined
}

export const labelOf = (lang: string, meta: string): string =>
  between(meta, '[', ']') ?? between(meta, 'title="', '"') ?? labels[lang] ?? lang

export const figure: ShikiTransformer = {
  name: 'dxfio-figure',
  root(root) {
    const meta = (this.options.meta as { __raw?: string } | undefined)?.__raw ?? ''
    const lang = String(this.options.lang)
    const title = /title="([^"]+)"/.exec(meta)?.[1]
    const children: ElementContent[] = [
      element('span', { className: ['code-lang'] }, [text(labels[lang] ?? lang)]),
      element('button', { type: 'button', className: ['code-copy'], dataCopy: '', ariaLabel: 'Copy the code' }, [
        text('Copy'),
      ]),
      ...(root.children as ElementContent[]),
    ]
    if (title) children.unshift(element('figcaption', {}, [text(title)]))
    return { type: 'root', children: [element('figure', { className: ['code'] }, children)] }
  },
}

export const highlighter = await createHighlighter({
  themes: [themes.light, themes.dark],
  langs: ['cpp', 'cmake', 'shellscript'],
})

const channels = (hex: string) => {
  const full = hex.length <= 5 ? `#${hex.slice(1).replace(/./g, (digit) => digit + digit)}` : hex
  return [1, 3, 5, 7].map((at, index) => (index < 3 || full.length > 7 ? Number.parseInt(full.slice(at, at + 2), 16) : 255))
}

const linear = (channel: number) => {
  const value = channel / 255
  return value <= 0.03928 ? value / 12.92 : ((value + 0.055) / 1.055) ** 2.4
}

const luminance = (rgb: number[]) => 0.2126 * linear(rgb[0]) + 0.7152 * linear(rgb[1]) + 0.0722 * linear(rgb[2])

const contrast = (one: number[], other: number[]) => {
  const [light, dark] = [luminance(one), luminance(other)].sort((a, b) => b - a)
  return (light + 0.05) / (dark + 0.05)
}

const hex = (rgb: number[]) => `#${rgb.map((channel) => Math.round(channel).toString(16).padStart(2, '0')).join('')}`

// A token color that reads below 4.5:1 on its theme's background is taken toward black,
// or toward white on a dark theme, until it reads; the others stay Ayu's own.
const legible = (name: string): Record<string, string> => {
  const theme = highlighter.getTheme(name)
  const back = channels(theme.bg)
  const toward = luminance(back) > 0.5 ? 0 : 255
  const colors = [theme.fg, ...theme.settings.map((one) => one.settings?.foreground)].filter(
    (color): color is string => typeof color === 'string' && color.startsWith('#'),
  )
  return Object.fromEntries(
    colors.flatMap((color) => {
      const [red, green, blue, alpha] = channels(color)
      let shown = [red, green, blue].map((channel, index) => channel * (alpha / 255) + back[index] * (1 - alpha / 255))
      if (contrast(shown, back) >= 4.5) return []
      while (contrast(shown, back) < 4.5) shown = shown.map((channel) => channel + (toward - channel) * 0.04)
      return [[color.toLowerCase(), hex(shown)]]
    }),
  )
}

for (const name of Object.values(themes)) {
  await highlighter.loadTheme({ ...highlighter.getTheme(name), colorReplacements: legible(name) })
}

export const paint = (code: string): string => highlighter.codeToHtml(code, { lang: 'cpp', themes, defaultColor: false })
