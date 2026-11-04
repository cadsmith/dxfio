import { link } from './route'

// Each group is a heading of the sidebar; a section may span several.
export const sections = [
  { id: 'guide', title: 'Guide', slugs: ['start'] },
  { id: 'tutorials', title: 'Drawings', slugs: ['reading', 'editing', 'saving', 'audit'] },
  { id: 'tutorials', title: 'Header and tables', slugs: ['units', 'layers', 'linetypes'] },
  {
    id: 'tutorials',
    title: 'Entities',
    slugs: ['colors', 'lines', 'polylines', 'splines', 'text', 'hatch', 'faces', 'xdata'],
  },
  { id: 'tutorials', title: 'Blocks and layouts', slugs: ['blocks', 'paper'] },
  { id: 'concepts', title: 'Concepts', slugs: ['document', 'releases', 'quality'] },
]

const titles: Record<string, string> = { guide: 'Guide', tutorials: 'Tutorials', concepts: 'Concepts' }

export const titleOf = (section: string) => titles[section] ?? section

// A section's link is its first page's, so each page has one address.
export const start = (section: string) => link(section, sections.find((one) => one.id === section)?.slugs[0] ?? '')
